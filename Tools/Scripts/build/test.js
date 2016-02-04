/**
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */
var path = require('path'),
	fs = require('fs'),
	async = require('async'),
	colors = require('colors'),
	wrench = require('wrench'),
	ejs = require('ejs'),
	windowslib = require('windowslib'),
	spawn = require('child_process').spawn,
	exec = require('child_process').exec,
	titanium = path.join(__dirname, 'node_modules', 'titanium', 'bin', 'titanium'),
	DIST_DIR = path.join(__dirname, '..', '..', '..', 'dist'),
	WINDOWS_DIST_DIR = path.join(DIST_DIR, 'windows'),
	MOCHA_ASSETS_DIR = path.join(__dirname, '..', '..', '..', 'Examples', 'NMocha', 'src', 'Assets'),
	// Constants
	WIN_8_1 = '8.1',
	WIN_10 = '10.0',
	MSBUILD_12 = '12.0',
	MSBUILD_14 = '14.0',
	DEFAULT_DEVICE_ID = '8-1-1',
	WP_EMULATOR = 'wp-emulator',
	MAX_RETRIES = 3,
	// Global vars
	hadWindowsSDK = false,
	projectDir = path.join(__dirname, 'mocha'),
	testResults,
	jsonResults;

/**
 * Installs the latest SDK from master branch remotely, sets it as the default
 * SDK. We'll be hacking it to add our locally built Windows SDK into it.
 *
 * @param next {Function} callback function
 **/
function installSDK(next) {
	var prc = spawn('node', [titanium, 'sdk', 'install', '-b', 'master', '-d']);
	prc.stdout.on('data', function (data) {
	   console.log(data.toString().trim());
	});
	prc.stderr.on('data', function (data) {
	   console.error(data.toString().trim());
	});

	prc.on('close', function (code) {
		if (code != 0) {
			next("Failed to install master SDK. Exit code: " + code);
		} else {
			next();
		}
	});
}

/**
 * Look up the full path to the SDK we just installed (the SDK we'll be hacking
 * to add our locally built Windows SDK into).
 *
 * @param next {Function} callback function
 **/
function getSDKInstallDir(next) {
	var prc = exec('node "' + titanium + '" info -o json -t titanium', function (error, stdout, stderr) {
		var out,
			selectedSDK;
		if (error !== null) {
			return next('Failed to get SDK install dir: ' + error);
		}

		out = JSON.parse(stdout);
		selectedSDK = out['titaniumCLI']['selectedSDK'];

		next(null, out['titanium'][selectedSDK]['path']);
	});
}

/**
 * Adds 'windows' into the list of supported platforms for a given SDK we're
 * hacking.
 *
 * @param sdkPath {String} path to the Titanium SDK we'll be hacking to copy our
 * 													locally built Windows SDK into
 * @param next {Function} callback function
 **/
function copyWindowsIntoSDK(sdkPath, next) {
	var dest = path.join(sdkPath, 'windows');
	if (fs.existsSync(dest)) {
		hadWindowsSDK = true;
		wrench.rmdirSyncRecursive(dest);
	}
	// TODO Be smarter about copying to speed it up? Only copy diff?
	wrench.copyDirSyncRecursive(WINDOWS_DIST_DIR, dest);
	next();
}

/**
 * Adds 'windows' into the list of supported platforms for a given SDK we're
 * hacking.
 *
 * @param sdkPath {String} path to the Titanium SDK we'll be hacking
 * @param next {Function} callback function
 **/
function addWindowsToSDKManifest(sdkPath, next) {
	var manifest = path.join(sdkPath, 'manifest.json');

	fs.readFile(manifest, function (err, data) {
		if (err) {
			next(err);
		}
		// append 'windows' to platforms array
		var json = JSON.parse(data);
		json['platforms'].push('windows');
		// Write new JSON back to file
		fs.writeFile(manifest, JSON.stringify(json), function (err) {
			if (err) {
				next(err);
			}
			next();
		});
	});
}

/**
 * Generates a new project for Windows. This will be the project we use for
 * running unit tests.
 *
 * @param next {Function} callback function
 **/
function generateWindowsProject(next) {
	var prc;
	// If the project already exists, wipe it
	if (fs.existsSync(projectDir)) {
		wrench.rmdirSyncRecursive(projectDir);
	}
	prc = spawn('node', [titanium, 'create', '--force', '--type', 'app', '--platforms', 'windows', '--name', 'mocha', '--id', 'com.appcelerator.mocha.testing', '--url', 'http://www.appcelerator.com', '--workspace-dir', __dirname, '--no-prompt']);
	prc.stdout.on('data', function (data) {
		console.log(data.toString());
	});
	prc.stderr.on('data', function (data) {
		console.log(data.toString());
	});
	prc.on('close', function (code) {
		if (code != 0) {
			next("Failed to create project");
		} else {
			next();
		}
	});
}

/**
 * Add required properties for our unit tests to the tiapp.xml
 *
 * @params sdkVersion {String} '8.1'||'10.0'
 * @param next {Function} callback function
 **/
function addTiAppProperties(sdkVersion, next) {
	var tiapp_xml = path.join(projectDir, 'tiapp.xml');

	// Not so smart but this should work...
	var content = [];
	fs.readFileSync(tiapp_xml).toString().split(/\r?\n/).forEach(function(line) {
		content.push(line);
		if (line.indexOf('<guid>') >= 0) {
			content.push('\t<property name="presetString" type="string">Hello!</property>');
			content.push('\t<property name="presetBool" type="bool">true</property>');
			content.push('\t<property name="presetInt" type="int">1337</property>');
			content.push('\t<property name="presetDouble" type="double">1.23456</property>');
			content.push('\t<windows><manifest><Capabilities><Capability Name=\"internetClient\" />');
			content.push('\t<Capability Name=\"contacts\" />');
			content.push('\t</Capabilities></manifest></windows>');
		}
	});
	fs.writeFileSync(tiapp_xml, content.join('\n'));

	next();
}

/**
 * Copies the assets from the NMocha example app into the generated project.
 *
 * @param next {Function} callback function
 **/
function copyMochaAssets(next) {
	var dest = path.join(projectDir, 'Resources');
	wrench.copyDirSyncRecursive(MOCHA_ASSETS_DIR, dest, {
		forceDelete: true
	});
	next();
}

/**
 * Grabs the id of the first emulator for a given sdk version
 *
 * @param sdkVersion {String} '8.1'|'10.0' Used to grab the list of valid
 *														emulators for the given sdk version
 * @param target {String} 'wp-emulator'|'ws-local'|'wp-device'
 * @param next {Function} callback function
 **/
function getDeviceId(sdkVersion, target, next) {
	windowslib.detect(function (err, results) {
		var deviceId = DEFAULT_DEVICE_ID;
		if (err) {
			next(err);
			return;
		}

		if (target === WP_EMULATOR) {
			deviceId = results.emulators && results.emulators[sdkVersion] && results.emulators[sdkVersion][0] && results.emulators[sdkVersion][0].udid;

			for(i in results.emulators[sdkVersion]) {
				var emulator = results.emulators[sdkVersion][i];
				if (/^10\.0\.10586/.test(emulator.uapVersion)) {
					deviceId = emulator.udid;
					console.log('Found 10.0.10586 Emulator : ' + deviceId);
					break;
				}
			}

		} else {
			deviceId = '0'; // assume device
			// TODO What about for ws-local?
		}

		next(null, deviceId);
	});
}

/**
 * Runs the build, should run the unit tests
 *
 * @param target {String} 'wp-emulator'|'ws-local'
 * @param deviceId {String} deviceId to launch on. '0' for actual physical
 *							device. Something like '8-1-1' for first Windows 8.1 emulator
 * @param count {Number} Tracks how many times we've tried to run this build.
 *											Used so we can finally abort after max retry count.
 * @param next {Function} callback function
 **/
function runBuild(target, deviceId, sdkVersion, count, next) {
	var prc,
		inResults = false,
		done = false;
	prc = spawn('node', [titanium, 'build', '--project-dir', projectDir, '--platform', 'windows', '--target', target, '--wp-sdk', sdkVersion, '--win-publisher-id', '13AFB724-65F2-4F30-8994-C79399EDBD80', '--device-id', deviceId, '--no-prompt', '--no-colors']);
	prc.stdout.on('data', function (data) {
		console.log(data.toString());
		var lines = data.toString().trim().match(/^.*([\n\r]+|$)/gm);
		for (var i = 0; i < lines.length; i++) {
			var str = lines[i],
				index = -1;

			if (inResults) {
				if ((index = str.indexOf('[INFO]')) != -1) {
					str = str.slice(index + 8).trim();
				}
				if ((index = str.indexOf('!TEST_RESULTS_STOP!')) != -1) {
					str = str.slice(0, index).trim();
					inResults = false;
					done = true; // we got the results we need, when we kill this process we'll move on
				}

				testResults += str;
				if (!inResults) {
					testResults = testResults.trim(); // for some reason, there's a leading space that is messing with everything!
					prc.kill();
					break;
				}
			}
			else if ((index = str.indexOf('!TEST_RESULTS_START!')) != -1) {
				inResults = true;
				testResults = str.substr(index + 20).trim();
			}

			// Handle when app crashes and we haven't finished tests yet!
			if ((index = str.indexOf('-- End application log ----')) != -1) {
				prc.kill(); // quit this build...
				if (count > MAX_RETRIES) {
					next("failed to get test results before log ended!"); // failed too many times
				} else {
					runBuild(target, deviceId, sdkVersion, count + 1, next); // retry
				}
			}
		}

	});
	prc.stderr.on('data', function (data) {
		console.log(data.toString());
	});

	prc.on('close', function (code) {
		if (done) {
			next(); // only move forward if we got results and killed the process!
		}
	});
}

/**
 * Converts the raw string outut from the test app into a JSON Object.
 *
 * @param testResults {String} Raw string output from the logs of the test app
 * @param next {Function} callback function
 */
function parseTestResults(testResults, next) {
	if (!testResults) {
		return next("Failed to retrieve any tests results!");
	}

	// preserve newlines, etc - use valid JSON
	testResults = testResults.replace(/\\n/g, "\\n")
			   .replace(/\\'/g, "\\'")
			   .replace(/\\"/g, '\\"')
			   .replace(/\\&/g, "\\&")
			   .replace(/\\r/g, "\\r")
			   .replace(/\\t/g, "\\t")
			   .replace(/\\b/g, "\\b")
			   .replace(/\\f/g, "\\f");
	// remove non-printable and other non-valid JSON chars
	testResults = testResults.replace(/[\u0000-\u0019]+/g,"");
	jsonResults = JSON.parse(testResults);
	next();
}

/**
 * Converts JSON results of unit tests into a JUnit test result XMl formatted file.
 *
 * @param jsonResults {Object} JSON containing results of the unit test output
 * @param next {Function} callback function
 */
function outputJUnitXML(jsonResults, next) {
	// We need to go through the results and separate them out into suites!
	var suites = {},
		keys = [],
		values = [],
		r = '';
	jsonResults.results.forEach(function(item) {
		var s = suites[item.suite] || {tests: [], suite: item.suite, duration: 0, passes: 0, failures: 0, start:''}; // suite name to group by
		s.tests.unshift(item);
		s.duration += item.duration;
		if (item.state == 'failed') {
			s.failures += 1;
		} else if (item.state == 'passed') {
			s.passes += 1;
		}
		suites[item.suite] = s;
	});
	keys = Object.keys(suites);
	values = keys.map(function(v) { return suites[v]; });
	var r = ejs.render('' + fs.readFileSync(path.join('.', 'junit.xml.ejs')),  { 'suites': values });

	// Write the JUnit XML to a file
	fs.writeFileSync(path.join(DIST_DIR, 'junit_report.xml'), r);
	next();
}

/**
 * Installs the SDK from master branch, copies a built Windows SDK into it, generates a Titanium mobile project
 * for Windows SDK, sets up the project, copies unit tests into it from Examples/NMocha/src/Aseets,
 * and then runs the project in a Windows simulator which will run the mocha unit tests. The test results are piped to
 * the CLi, which takes them and generates a JUnit test result XML report for the Jenkins build machine.
 *
 * @param sdkVersion {String} '8.1'|'10.0'
 * @param msbuild {String} '12.0'|'14.0' (Visual Studio 2013 or 2015)
 * @param target {String} 'wp-emulator'|'ws-local'
 * @param callback {Function} callback function
 */
function test(sdkVersion, msbuild, target, deviceId, callback) {
	var sdkPath;
	async.series([
		function (next) {
			// If this is already installed we don't re-install, thankfully
			console.log("Installing SDK from master branch");
			installSDK(next);
		},
		function (next) {
			getSDKInstallDir(function (err, installPath) {
				if (err) {
					return next(err);
				}
				sdkPath = installPath;
				next();
			});
		},
		function (next) {
			console.log("Copying built Windows SDK into master SDK");
			copyWindowsIntoSDK(sdkPath, next);
		},
		function (next) {
			if (hadWindowsSDK) {
				next();
			} else {
				addWindowsToSDKManifest(sdkPath, next);
			}
		},
		function (next) {
			console.log("Generating Windows project");
			generateWindowsProject(next);
		},
		function (next) {
			console.log("Adding properties for tiapp.xml");
			addTiAppProperties(sdkVersion, next);
		},
		function (next) {
			console.log("Copying test scripts into project");
			copyMochaAssets(next);
		},
		function (next) {
			if (deviceId) {
				return next();
			}

			console.log('Detecting simulator');
			getDeviceId(sdkVersion, target, function (err, id) {
				if (err) {
					return next(err);
				}
				deviceId = id;
				next();
			});
		},
		function (next) {
			console.log("Launching test project in simulator");
			runBuild(target, deviceId, sdkVersion, 1, next);
		},
		function (next) {
			parseTestResults(testResults, next);
		},
		function (next) {
			outputJUnitXML(jsonResults, next);
		}
	], callback);
}

// public API
exports.test = test;

// When run as single script.
if (module.id === ".") {
	(function () {
		var program = require('commander');

		// TODO Allow specifying what device id?
		program
			.version('0.0.1')
			.option('-m, --msbuild [version]', 'Use a specific version of MSBuild', /^(12\.0|14\.0)$/, MSBUILD_12)
			.option('-s, --sdk-version [version]', 'Target a specific Windows SDK version [version]', /^(8\.1|10\.0)$/, WIN_8_1)
			.option('-T, --target [target]', 'Target a specific deploy target [target]', /^wp\-emulator|ws\-local|wp\-device$/, WP_EMULATOR)
			.option('-C, --device-id [udid]', 'Target a specific device/emulator')
			.parse(process.argv);

		// When doing win 10, it has to use msbuild 14
		if (program.sdkVersion == WIN_10) {
			// TODO Log warning if they used msbuild 12!
			program.msbuild = MSBUILD_14;
		}
		test(program.sdkVersion, program.msbuild, program.target, program.deviceId, function (err, results) {
			if (err) {
				console.error(err.toString().red);
				process.exit(1);
			} else {
				process.exit(0);
			}
		});
	})();
}
