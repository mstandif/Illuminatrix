# coding=utf-8
from __future__ import absolute_import

import octoprint.plugin
import flask
import os
import serial
from subprocess import call,Popen

class IlluminumPlugin(octoprint.plugin.StartupPlugin,
			octoprint.plugin.TemplatePlugin,
			octoprint.plugin.EventHandlerPlugin,
			octoprint.plugin.BlueprintPlugin,
			octoprint.plugin.SettingsPlugin):

#	def __init__(self):
#		pass

	def setSerialConfig(self, port):
		try:
			if port is not None:
				Popen("stty -F "+port+" cs8 9600 min 100 time 2 -iuclc -ixany -imaxbel -iutf8 -parenb -parodd cs8 -hupcl -cstopb cread clocal -crtscts -ignbrk brkint ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl ixon -ixoff -iuclc -ixany -imaxbel -iutf8 -opost -olcuc -ocrnl -onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0 -isig -icanon iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke &", shell=True)
		except:
			return

	def issueCommand(self, cmd_str):
		try:
			port = self._settings.get(["Port"])
			self.setSerialConfig(port)
			Popen("echo -n \";"+cmd_str+";\" > "+port, shell=True)
			return flask.make_response("Illuminum "+cmd_str+" mode", 750)
		except:
			return

	##StartupPlugin
	def on_after_startup(self):
		self._logger.info("Illuminum plugin lives")

	##TemplatePlugin
	def get_template_configs(self):
		return [
			dict(type="tab", custom_bindings=False),
			dict(type="settings", custom_bindings=False), 
			#dict(type="tab", template="Illuminum_tab.jinja2")
			#dict(type="settings", template="Illuminum_settings.jinja2")
		]

	##BlueprintPlugin
	@octoprint.plugin.BlueprintPlugin.route("/on", methods=["GET"])
	def on(self):
		return self.issueCommand("ON")

	##BlueprintPlugin
	@octoprint.plugin.BlueprintPlugin.route("/off", methods=["GET"])
	def off(self):
		return self.issueCommand("OFF")

	##BlueprintPlugin
	@octoprint.plugin.BlueprintPlugin.route("/white", methods=["GET"])
	def white(self):
		return self.issueCommand("WHITE")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/red", methods=["GET"])
        def red(self):
                return self.issueCommand("RED")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/green", methods=["GET"])
        def green(self):
                return self.issueCommand("GREEN")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/blue", methods=["GET"])
        def blue(self):
                return self.issueCommand("BLUE")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/lightblue", methods=["GET"])
        def lightblue(self):
                return self.issueCommand("LIGHTBLUE")


        ##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/purple", methods=["GET"])
        def purple(self):
                return self.issueCommand("PURPLE")

        ##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/yellow", methods=["GET"])
        def yellow(self):
                return self.issueCommand("YELLOW")

        ##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/standby", methods=["GET"])
        def standby(self):
                return self.issueCommand("STANDBY")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/cycleon", methods=["GET"])
        def cycleon(self):
                return self.issueCommand("CYCLEON")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/cyclewhite", methods=["GET"])
        def cyclewhite(self):
                return self.issueCommand("CYCLEWHITE")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/cyclered", methods=["GET"])
        def cyclered(self):
                return self.issueCommand("CYCLERED")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/cyclegreen", methods=["GET"])
        def cyclegreen(self):
                return self.issueCommand("CYCLEGREEN")

	##BlueprintPlugin
        @octoprint.plugin.BlueprintPlugin.route("/cycleblue", methods=["GET"])
        def cyclewblue(self):
                return self.issueCommand("CYCLEBLUE")

	def is_blueprint_protected(self):
		return False


	#### EVENTS #####################################################
	##EventHandlerPlugin
	def on_event(self, event, payload):
		command = self._settings.get([event])
		if command is not None:
			self.issueCommand(command)

	#### SETTINGS ###################################################
	def get_settings_defaults(self):
		return dict(
			Disconnected="OFF",
			Connected="STANDBY",
			PrintStarted="CYCLEWHITE",
			PrintCancelled="STANDBY",
			PrintFailed="RED",
			PrintPaused="YELLOW",
			PrintResumed="WHITE",
			PrintDone="STANDBY",
			Home="CYCLEGREEN",
			Port="/dev/ttyAMA0",
		)

	def get_template_configs(self):
		return [
			dict(type="settings", custom_bindings=False),
			dict(type="tab", custom_bindings=False),
		]

# If you want your plugin to be registered within OctoPrint under a different name than what you defined in setup.py
# ("OctoPrint-PluginIlluminum"), you may define that here. Same goes for the other metadata derived from setup.py that
# can be overwritten via __plugin_xyz__ control properties. See the documentation for that.

__plugin_name__ = "Illuminum"
__plugin_implementation__ = IlluminumPlugin()
