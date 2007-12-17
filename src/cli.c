#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libmpd/debug_printf.h>

#include "mpd.h"

/// maximum option count for one action
#define MAX_OPTS 5

/// maximum length of an option
#define MAXLEN 30

/// error codes
enum {
    ERROR_UNKNOWN = 1,
    ERROR_MALFORMED,
    ERROR_MAX_OPTS_EXCEEDED,
    ERROR_MPD_NOT_REACHABLE,
    ERROR_MPD_FAILED,
    ERROR_NOT_IMPLEMENTED
};

/// error indicator for #parse_options and #run
static int err_num = 0;
/// maximum error string length
#define ERR_LEN 50
/// error string
char err_str[ERR_LEN];

/// known actions
static char *actions[] = { "play", "pause", "stop", "next", "prev", "playlist" };
/// number of known actions
#define ACTION_COUNT 6

/**
 * print a usage message
 * \param progname supply argv[0] for better readability
 * of the output
 */
void usage(char *progname) {
    // TODO: change this to use the actions list
    printf("usage: %s <options>\n", progname);
    puts(  "  <options>:");
    puts(  "    play [num] -- play last song / num-th song");
    puts(  "    pause      -- pause playback");
    puts(  "    stop       -- stop playback");
    puts(  "    next       -- next song");
    puts(  "    prev       -- previous song");
    puts(  "    playlist   -- show playlist");
    return;
}

/**
 * print an error message, error codes are stored in #err_num
 * \param progname supply argv[0] for better readability of the messages
 */
void error(char *progname) {
    
    printf("%s: error: ", progname);

    switch(err_num) {
	// parse_options errors:
	case ERROR_UNKNOWN:
	    printf("%s: ", err_str);
	    puts("unknown action.");
	    usage(progname);
	    break;
	case ERROR_MALFORMED:
	    puts("malformed arguments to requested action.");
	    usage(progname);
	    break;
	case ERROR_MAX_OPTS_EXCEEDED:
	    printf("more than %d options supplied.", MAX_OPTS);
	    break;
	// run errors:
	case ERROR_MPD_NOT_REACHABLE:
	    puts("mpd was not reachable.");
	    break;
	case ERROR_MPD_FAILED:
	    puts("mpd returned MPD_STATUS_FAILED.");
	    break;
	case ERROR_NOT_IMPLEMENTED:
	    puts("this function is not yet implemented.");
	    break;
	default:
	    puts("new error code or sth went seriously wrong");
	    break;
    }
    puts("\n");

    // clear error code
    err_num = 0;

    return;
}

/**
 * parse command line options
 * \param argc argc parameter from main
 * \param argv argv parameter from main
 * \param optlist place to store processed options (will be NULL-terminated)
 * Caller needs to free any non-NULL entries in optlist
 * \return number of processed options
 */
int parse_options(int argc, char *argv[], char *optlist[]) {
	
    int opt_count = 0;
    int i;
    int song_num;

    // jump program name (argv[0])
    char *progname = *argv;
    argv++;

    // process all command line options
    while(--argc) {
	if(opt_count > MAX_OPTS) {
	    err_num=ERROR_MAX_OPTS_EXCEEDED;
	    error(progname);
	    exit(EXIT_FAILURE);
	}
	optlist[opt_count ++] = (char*)malloc(MAXLEN*sizeof(char));
	// malloc error 
	if(optlist[opt_count-1] == NULL) {
	    perror(progname);
	    exit(EXIT_FAILURE);
	}
	// check if known action
	for(i=0;(i<ACTION_COUNT)&&(strcmp(actions[i],*argv)!=0);++i);
	// unknown action error handling
	if(i>=ACTION_COUNT) {
	    err_num = ERROR_UNKNOWN;
	    strncpy(err_str,*argv,ERR_LEN);
	    continue;
	}
	// determine which action
	switch(i) {
	    case 0: // play
		--argc;
		if(argc) { // more arguments, get number of song to play
		    ++argv;
		    song_num = atoi(*argv);
		    snprintf(optlist[opt_count-1], MAXLEN-1, "play %d", song_num);
		}
		else {
		    snprintf(optlist[opt_count-1], MAXLEN-1, "play");
		    // if we're here, argc was zero
		    return opt_count;
		}
		break;
	    case 1: // pause
		snprintf(optlist[opt_count-1], MAXLEN-1, "pause");
		break;
	    case 2: // stop
		snprintf(optlist[opt_count-1], MAXLEN-1, "stop");
		break;
	    case 3: // next
		snprintf(optlist[opt_count-1], MAXLEN-1, "next");
		break;
	    case 4: // previous
		snprintf(optlist[opt_count-1], MAXLEN-1, "prev");
		break;
	    case 5: // playlist
		snprintf(optlist[opt_count-1], MAXLEN-1, "playlist");
		break;
	    default: // unknown action error
		err_num = ERROR_UNKNOWN;
		break;
	}
	++argv;
    }

    return opt_count;
}

/**
 * execute parsed options
 * \param optlist optlist aqcuired by calling #parse_options
 * \return 1 if successful, 0 if error
 */
int run(char *optlist[], int opt_count) {
   
    // for now ignore anything but the first optlist entry

    char *optlist_entry;
    int i;
    int song_num;
    int status;

    for(i=0;(i<ACTION_COUNT)&&(strcmp(actions[i],*optlist)!=0);++i);
    if(i>=ACTION_COUNT) {
	// only possibility this can happen is when 
	// calling play with a song number
	i=6;
	optlist_entry = *optlist;
	while(*(optlist_entry++)!=' '); // search for blank in optlist string
	song_num = atoi(optlist_entry);
    }

    // connect to mpd
    mpd_info.obj = mpd_new(HOST, PORT, PASSWORD);
    mpd_connect(mpd_info.obj);
    if(!mpd_is_connected(mpd_info.obj)) {
	err_num = ERROR_MPD_NOT_REACHABLE;
	return 0;
    }
    
    //debug_set_level(DEBUG_WARNING);

    // do what the user requested
    switch(i) {
	case 0: // play
	    status = mpd_player_play(mpd_info.obj);
	    if(status != MPD_OK) {
		err_num = ERROR_MPD_FAILED;
		return 0;
	    }
	    break;
	case 6: // play song_num
	    status = mpd_player_play_id(mpd_info.obj, song_num);
	    if(status != MPD_OK) {
		err_num = ERROR_MPD_FAILED;
		return 0;
	    }
	    break;
	case 1: // pause
	    // really *ugly* hack, everything but mpd_player_pause works
	    // w/o mpd_player_get_state() called before the command...
	    mpd_player_get_state(mpd_info.obj);
	    status = mpd_player_pause(mpd_info.obj);
	    if(status != MPD_OK) {
		err_num = ERROR_MPD_FAILED;
		return 0;
	    }
	    break;
	case 2: // stop
	    status = mpd_player_stop(mpd_info.obj);
	    if(status != MPD_OK) {
		err_num = ERROR_MPD_FAILED;
		return 0;
	    }
	    break;
	case 3: // next
	    status = mpd_player_next(mpd_info.obj);
	    if(status != MPD_OK) {
		err_num = ERROR_MPD_FAILED;
		return 0;
	    }
	    break;
	case 4: // previous
	    status = mpd_player_prev(mpd_info.obj);
	    if(status != MPD_OK) {
		err_num = ERROR_MPD_FAILED;
		return 0;
	    }
	    break;
	case 5: // playlist
	    // playlist display
	    err_num = ERROR_NOT_IMPLEMENTED;
	    return 0;
	default:
	    err_num = ERROR_UNKNOWN;
	    return 0;
    }

    mpd_disconnect(mpd_info.obj);

    return 1;
}

int main(int argc, char *argv[]) {

    char *optlist[MAX_OPTS];
    int i;
    int opt_count;

    for(i=0;i<MAX_OPTS;++i)
	optlist[i] = NULL;

    opt_count = parse_options(argc, argv, optlist);
    if(err_num) {
	error(argv[0]);
	return EXIT_FAILURE;
    }

    if(opt_count > MAX_OPTS || opt_count <= 0) {
	usage(argv[0]);
	// cleanup 
	for(i=0;i<opt_count;++i)
	    free(optlist[i]);
	return EXIT_FAILURE;
    }

    // setup mpd info
    mpd_info.msi.host = g_string_new("localhost");
    mpd_info.msi.port = 6600;
    mpd_info.msi.timeout = 15;
    mpd_info.msi.pw = g_string_new("");

    if(!run(optlist, opt_count)) {
	error(argv[0]);
	return EXIT_FAILURE;
    }

    // cleanup
    for(i=0;i<opt_count;++i)
	free(optlist[i]);
    
    mpd_disconnect(mpd_info.obj);
    mpd_free(mpd_info.obj);

    g_string_free(mpd_info.msi.host, TRUE);
    g_string_free(mpd_info.msi.pw, FALSE);

    return EXIT_SUCCESS;
}

/* vim:sts=4:shiftwidth=4
 */
