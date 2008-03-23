/*
 * musicus_song.c - implementatiion
 */

#include "musicus_song.h"

struct _MusicusSongPrivate {
    GString *song_name;
}

GType musicus_song_get_type(void) {
    static GType type = 0;
    if (type == 0) {
	static const GTypeInfo info = {
	    sizeof (MusicusSongClass),
	    NULL, /* base init */
	    NULL, /* base finalize */
	    (GClassInitFunc)musicus_song_class_init, /* class init */
	    NULL, /* class finalize */
	    NULL, /* class data */
	    sizeof (MusicusSong),
	    0,
	    (GInstanceInitFunc)musicus_song_init, /* instance init */
	};
	type = g_type_register_static (G_TYPE_OBJECT, "MusicusSongType",
				       &info, 0);
    }

    return type;
}

/* vim:sts=4:shiftwidth=4
 */
