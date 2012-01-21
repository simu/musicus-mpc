const St = imports.gi.St;
const Lang = imports.lang;
const PanelMenu = imports.ui.panelMenu;
const PopupMenu = imports.ui.popupMenu;
const Main = imports.ui.main;
const GLib = imports.gi.GLib;
//const Gio = imports.gi.Gio;
const Util = imports.misc.util;
const Mainloop = imports.mainloop;
//gnome 3.0
const Panel = imports.ui.panel;

function Musicus() {
    this._init();
}

Musicus.prototype = {
    __proto__: PanelMenu.Button.prototype,

    _init: function(){
        PanelMenu.Button.prototype._init.call(this, 0.0);

        this.lang = {
            'play' : 'Play', 
            'pause' : 'Pause', 
            'stop' : 'Stop',
			'prev' : 'Previous',
			'next' : 'Next'
        };
        this.statusLabel = new St.Label({
            text: "--", 
            style_class: "sm-label"
        });

        // destroy all previously created children, and add our statusLabel
        this.actor.get_children().forEach(function(c) {
            c.destroy()
        });
        this.actor.add_actor(this.statusLabel);
		Main.panel._centerBox.add(this.actor, {y_fill:true });

        this._control = this._detectMpc();

        if(this._control===null){
            this.title='Warning';
            this.content='Please install mpc. If it doesn\'t help, click here to report!';
        }

        this._update_status();
        //update every 5 seconds, todo: prefs
        event = GLib.timeout_add_seconds(0, 5, Lang.bind(this, function () {
            this._update_status();
            return true;
        }));
    },

    _detectMpc: function(){
        //detect if sensors is installed
        let ret = GLib.spawn_command_line_sync("which mpc");
        if ( (ret[0]) && (ret[3] == 0) ) {//if yes
            return ret[1].toString().split("\n", 1)[0];//find the path of the sensors
        }
        return null;
    },

    _update_status: function() {
        let items = new Array();
        if (this._control){
			let raw_mpc_output = GLib.spawn_command_line_sync(this._control)[1].toString();
			this._parse_mpc_output(raw_mpc_output);
			this._updateMenu();
        }
		else {
			this._updateMenu("Install mpc to use this extension!");
	},

	_parse_mpc_output: function(raw_output) {
		let lines = raw_output.split('\n');
		this._songTitle = lines[0];

		let state_line = lines[1].split(' ');
		this._playState = state_line[0];
		items.push(state_line[0]);
		let nos = state_line[1].split('/');
		this._songNo = nos[0][1:];
		this._playlistSongCount = nos[1]
		let times = state_line[2].split('/');
		this._currTime = times[0];
		this._totalTime= times[1];

		let setting_line = lines[2].split(/   /);
		this._volume = setting_line[0].split(/: /)[1].split(/%/)[0];
		this._repeat  = setting_line[1].match(/off/)==null;
		this._random  = setting_line[2].match(/off/)==null;
		this._single  = setting_line[3].match(/off/)==null;
		this._consume = setting_line[4].match(/off/)==null;
	},

	_updateMenu: function(msg=null) {
        this.statusLabel.set_text("MPD: " + state_line[0]);
		// clear box
        this.menu.box.get_children().forEach(function(c) {
            c.destroy()
        });
		if (msg != null) {
			this.menu.addMenuItem(new PopupMenu.PopupMenuItem(msg));
		}
		else {
			let section = new PopupMenu.PopupMenuSection("MPD state");
			if (items.length>0){
				let item;
				for each (let itemText in items){
					item = new PopupMenu.PopupMenuItem("");
					item.addActor(new St.Label({
						text:itemText.toString(), 
						style_class: "sm-label"
					}));
					section.addMenuItem(item);
				}
			}
			this.menu.addMenuItem(section);
		}
		Main.panel._centerBox.add(this.actor, {y_fill:true});
		Main.panel._menus.addMenu(this.menu);
    }
}

//for debugging
function debug(a){
    global.log(a);
    //Util.spawn(['echo',a]);
}

function start(extMeta) {
    /*let userExtensionLocalePath = extMeta.path + '/locale';
    Gettext.bindtextdomain("places_button", userExtensionLocalePath);
    Gettext.textdomain("places_button");*/
	new Musicus();
}

function init(extensionMeta) {
	start(extensionMeta)
}

//gnome3.0
function main(extensionMeta) {
	start(extensionMeta)
}

let musicus;
let event=null;

function enable() {
    musicus = new Musicus();
}

function disable() {
    musicus.destroy();
	Mainloop.source_remove(event);
	indicator = null;
}
