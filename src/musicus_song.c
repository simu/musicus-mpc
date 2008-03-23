/*
 * musicus_song.c - implementatiion
 */

#include "musicus_song.h"

struct _MusicusSongPrivate {
    GString *song_name;
    gboolean dispose_has_run;
};

static GObjectClass *parent_class = NULL;

/* internal methods */
static void musicus_song_class_init (MusicusSongClass *klass, gpointer class_data);
static void musicus_song_init (GTypeInstance *instance, gpointer class);
static void musicus_song_dispose (GObject *object);
static void musicus_song_finalize (GObject *object);

const gchar *musicus_song_get_song_name(MusicusSong *song) {
    if(song->priv->dispose_has_run)
	return NULL;
    return song->priv->song_name->str;
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

static void musicus_song_class_init (MusicusSongClass *klass, gpointer class_data) {

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = musicus_song_dispose;
    gobject_class->finalize = musicus_song_finalize;

    parent_class = g_type_class_peek_parent (klass);

    g_type_class_add_private (klass, sizeof(MusicusSongPrivate));
    return;
}

static void musicus_song_init (GTypeInstance *instance, gpointer class) {
    MusicusSong *self = MUSICUS_SONG (instance);
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MUSICUS_SONG_TYPE, MusicusSongPrivate);
    self->priv->song_name = g_string_new("");
    self->priv->dispose_has_run = FALSE;
    return;
}

static void musicus_song_dispose (GObject *object) {

    MusicusSong *self = MUSICUS_SONG (object);

    /* if dispose did run already, return */
    if(self->priv->dispose_has_run) {
	return;
    }

    /* Make sure dispose does not run twice */
    self->priv->dispose_has_run = TRUE;

    /* free song_name */
    g_string_free(self->priv->song_name, TRUE);

    G_OBJECT_CLASS (parent_class)->dispose (object);

    return;
}

static void musicus_song_finalize (GObject *object) {

    MusicusSong *self = MUSICUS_SONG(object);

    G_OBJECT_CLASS(parent_class)->finalize (object);

    return;
}

/* vim:sts=4:shiftwidth=4
 */
