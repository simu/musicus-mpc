#include <stdlib.h>
#include <gtk/gtk.h>
#include "media_browser.h"

void cleanup() {

	mpd_disconnect(mpd_info.obj);

	mpd_free(mpd_info.obj);

	g_string_free(mpd_info.msi.host, TRUE);
	g_string_free(mpd_info.msi.pw, TRUE);

	return;
}

void quit_cb(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
    return;
}

int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv);

	log_file = stdout;
	err_file = stderr;

	mpd_info.msi.host = g_string_new("localhost");
	mpd_info.msi.port = 6600;
	mpd_info.msi.timeout = 15;
	mpd_info.msi.pw = g_string_new("");

	mpd_info.obj = mpd_new(HOST,PORT,PASSWORD);

	mpd_connect(mpd_info.obj);

	media_browser_new();

	g_signal_connect(G_OBJECT(MbWin.win), "destroy",
			 G_CALLBACK(quit_cb), NULL);
	g_signal_connect(G_OBJECT(MbWin.win), "delete_event",
			 G_CALLBACK(quit_cb), NULL);

	media_browser_window_show();

	gtk_main();

	printf("after main");

	cleanup();

	return EXIT_SUCCESS;
}
