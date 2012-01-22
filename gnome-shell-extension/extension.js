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

/** {{{ sprintf() for JavaScript 0.7-beta1
http://www.diveintojavascript.com/projects/javascript-sprintf

Copyright (c) Alexandru Marasteanu <alexaholic [at) gmail (dot] com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of sprintf() for JavaScript nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Alexandru Marasteanu BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

var sprintf = (function() {
	function get_type(variable) {
		return Object.prototype.toString.call(variable).slice(8, -1).toLowerCase();
	}
	function str_repeat(input, multiplier) {
		for (var output = []; multiplier > 0; output[--multiplier] = input) {/* do nothing */}
		return output.join('');
	}

	var str_format = function() {
		if (!str_format.cache.hasOwnProperty(arguments[0])) {
			str_format.cache[arguments[0]] = str_format.parse(arguments[0]);
		}
		return str_format.format.call(null, str_format.cache[arguments[0]], arguments);
	};

	str_format.format = function(parse_tree, argv) {
		var cursor = 1, tree_length = parse_tree.length, node_type = '', arg, output = [], i, k, match, pad, pad_character, pad_length;
		for (i = 0; i < tree_length; i++) {
			node_type = get_type(parse_tree[i]);
			if (node_type === 'string') {
				output.push(parse_tree[i]);
			}
			else if (node_type === 'array') {
				match = parse_tree[i]; // convenience purposes only
				if (match[2]) { // keyword argument
					arg = argv[cursor];
					for (k = 0; k < match[2].length; k++) {
						if (!arg.hasOwnProperty(match[2][k])) {
							throw(sprintf('[sprintf] property "%s" does not exist', match[2][k]));
						}
						arg = arg[match[2][k]];
					}
				}
				else if (match[1]) { // positional argument (explicit)
					arg = argv[match[1]];
				}
				else { // positional argument (implicit)
					arg = argv[cursor++];
				}

				if (/[^s]/.test(match[8]) && (get_type(arg) != 'number')) {
					throw(sprintf('[sprintf] expecting number but found %s', get_type(arg)));
				}
				switch (match[8]) {
					case 'b': arg = arg.toString(2); break;
					case 'c': arg = String.fromCharCode(arg); break;
					case 'd': arg = parseInt(arg, 10); break;
					case 'e': arg = match[7] ? arg.toExponential(match[7]) : arg.toExponential(); break;
					case 'f': arg = match[7] ? parseFloat(arg).toFixed(match[7]) : parseFloat(arg); break;
					case 'o': arg = arg.toString(8); break;
					case 's': arg = ((arg = String(arg)) && match[7] ? arg.substring(0, match[7]) : arg); break;
					case 'u': arg = Math.abs(arg); break;
					case 'x': arg = arg.toString(16); break;
					case 'X': arg = arg.toString(16).toUpperCase(); break;
				}
				arg = (/[def]/.test(match[8]) && match[3] && arg >= 0 ? '+'+ arg : arg);
				pad_character = match[4] ? match[4] == '0' ? '0' : match[4].charAt(1) : ' ';
				pad_length = match[6] - String(arg).length;
				pad = match[6] ? str_repeat(pad_character, pad_length) : '';
				output.push(match[5] ? arg + pad : pad + arg);
			}
		}
		return output.join('');
	};

	str_format.cache = {};

	str_format.parse = function(fmt) {
		var _fmt = fmt, match = [], parse_tree = [], arg_names = 0;
		while (_fmt) {
			if ((match = /^[^\x25]+/.exec(_fmt)) !== null) {
				parse_tree.push(match[0]);
			}
			else if ((match = /^\x25{2}/.exec(_fmt)) !== null) {
				parse_tree.push('%');
			}
			else if ((match = /^\x25(?:([1-9]\d*)\$|\(([^\)]+)\))?(\+)?(0|'[^$])?(-)?(\d+)?(?:\.(\d+))?([b-fosuxX])/.exec(_fmt)) !== null) {
				if (match[2]) {
					arg_names |= 1;
					var field_list = [], replacement_field = match[2], field_match = [];
					if ((field_match = /^([a-z_][a-z_\d]*)/i.exec(replacement_field)) !== null) {
						field_list.push(field_match[1]);
						while ((replacement_field = replacement_field.substring(field_match[0].length)) !== '') {
							if ((field_match = /^\.([a-z_][a-z_\d]*)/i.exec(replacement_field)) !== null) {
								field_list.push(field_match[1]);
							}
							else if ((field_match = /^\[(\d+)\]/.exec(replacement_field)) !== null) {
								field_list.push(field_match[1]);
							}
							else {
								throw('[sprintf] huh?');
							}
						}
					}
					else {
						throw('[sprintf] huh?');
					}
					match[2] = field_list;
				}
				else {
					arg_names |= 2;
				}
				if (arg_names === 3) {
					throw('[sprintf] mixing positional and named placeholders is not (yet) supported');
				}
				parse_tree.push(match);
			}
			else {
				throw('[sprintf] huh?');
			}
			_fmt = _fmt.substring(match[0].length);
		}
		return parse_tree;
	};

	return str_format;
})();

var vsprintf = function(fmt, argv) {
	argv.unshift(fmt);
	return sprintf.apply(null, argv);
};

/** endof sprintf for javascript }}} **/

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
		this.items = {
			'Title %s'        : ['_songTitle'],
			'Song %d of %d'   : ['_songNo','_playlistSongCount'],
			'Playtime: %s/%s' : ['_currTime', '_totalTime'],
			'Volume: %d%%'    : ['_volume'],
			'Repeat: %s   Random: %s   Single: %s   Consume: %s' : ['_repeat', '_random', '_single', '_consume']
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
        if (this._control){
			let raw_mpc_output = GLib.spawn_command_line_sync(this._control)[1].toString();
			this._parse_mpc_output(raw_mpc_output);
			this._updateMenu(null);
        }
		else {
			this._updateMenu("Install mpc to use this extension!");
		}
	},

	_parse_mpc_output: function(raw_output) {
		let lines = raw_output.split('\n');
		if (lines.length == 2) {
			this._playState = "[stopped]";
			this._songTitle="n/a";
			this._volume = 0;
			this._songNo = -1;
			this._playlistSongCount = -1;
			this._currTime = -1;
			this._totalTime= -1;
			let setting_line = lines[0].split(/   /);
			this._volume = parseInt(setting_line[0].split(/: /)[1].split(/%/)[0]);
			this._repeat  = setting_line[1].match(/off/)==null;
			this._random  = setting_line[2].match(/off/)==null;
			this._single  = setting_line[3].match(/off/)==null;
			this._consume = setting_line[4].match(/off/)==null;
		}
		else {
			this._songTitle = lines[0];

			// does not always work properly.
			let state_line = lines[1].split(' ');
			this._playState = state_line[0];
			let nos = state_line[1].split('/');
			this._songNo = parseInt(nos[0].split('#')[1]);
			this._playlistSongCount = parseInt(nos[1]);
			let times = state_line[2].split('/');
			this._currTime = times[0];
			this._totalTime= times[1];

			let setting_line = lines[2].split(/   /);
			this._volume = parseInt(setting_line[0].split(/: /)[1].split(/%/)[0]);
			this._repeat  = setting_line[1].match(/off/)==null;
			this._random  = setting_line[2].match(/off/)==null;
			this._single  = setting_line[3].match(/off/)==null;
			this._consume = setting_line[4].match(/off/)==null;
		}
	},

	_updateMenu: function(msg) {
        this.statusLabel.set_text("MPD: " + this._playState);
		// clear box
        this.menu.box.get_children().forEach(function(c) {
            c.destroy()
        });
		if (msg != null) {
			this.menu.addMenuItem(new PopupMenu.PopupMenuItem(msg));
		}
		else {
			let section = new PopupMenu.PopupMenuSection("MPD state");
			let item;
			for (let itemText in this.items){
				var t = this;
				let _items = this.items[itemText].map(function(x){return t[x];});
				itemStr = vsprintf(itemText.toString(), _items);
				item = new PopupMenu.PopupMenuItem("");
				item.addActor(new St.Label({
					text:itemStr.toString(),
					style_class: "sm-label"
				}));
				section.addMenuItem(item);
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
