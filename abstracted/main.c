/* main.c
 *
 * Copyright (C) 2010 Christian Hergert <chris@dronelabs.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/sysinfo.h>
#include <ctype.h>

#include "uber.h"

typedef struct
{
	guint    len;
	gdouble *total;
	glong   *last_user;
	glong   *last_idle;
	glong   *last_system;
	glong   *last_nice;
} CpuInfo;

static guint        gdk_event_count  = 0;
static CpuInfo      cpu_info         = { 0 };
static const gchar *default_colors[] = { "#73d216",
                                         "#f57900",
                                         "#3465a4",
                                         "#ef2929",
                                         "#75507b",
                                         "#ce5c00",
                                         "#c17d11",
                                         "#ce5c00",
                                         NULL };

static void
gdk_event_hook (GdkEvent *event, /* IN */
                gpointer  data)  /* IN */
{
	gdk_event_count++;
	gtk_main_do_event(event);
}

static gboolean
get_xevent_info (UberLineGraph *graph,     /* IN */
                 guint          line,      /* IN */
                 gdouble       *value,     /* OUT */
                 gpointer       user_data) /* IN */
{
	switch (line) {
	case 1:
		*value = gdk_event_count;
		gdk_event_count = 0;
		break;
	default:
		g_assert_not_reached();
	}
	return TRUE;
}

static gboolean
get_cpu_info (UberLineGraph *graph,     /* IN */
              guint          line,      /* IN */
              gdouble       *value,     /* OUT */
              gpointer       user_data) /* IN */
{
	g_assert_cmpint(line, >, 0);
	g_assert_cmpint(line, <=, cpu_info.len);

	*value = cpu_info.total[line - 1];
	return TRUE;
}

static void
next_cpu_info (void)
{
	gchar cpu[64] = { 0 };
	glong user;
	glong system;
	glong nice;
	glong idle;
	glong user_calc;
	glong system_calc;
	glong nice_calc;
	glong idle_calc;
	gchar *buf = NULL;
	glong total;
	gchar *line;
	gint ret;
	gint id;
	gint i;

	if (!cpu_info.len) {
#if __linux__
		cpu_info.len = get_nprocs();
#else
#error "Your platform is not supported"
#endif
		g_assert(cpu_info.len);
		/*
		 * Allocate data for sampling.
		 */
		cpu_info.total = g_new0(gdouble, cpu_info.len);
		cpu_info.last_user = g_new0(glong, cpu_info.len);
		cpu_info.last_idle = g_new0(glong, cpu_info.len);
		cpu_info.last_system = g_new0(glong, cpu_info.len);
		cpu_info.last_nice = g_new0(glong, cpu_info.len);
	}

	if (g_file_get_contents("/proc/stat", &buf, NULL, NULL)) {
		line = buf;
		for (i = 0; buf[i]; i++) {
			if (buf[i] == '\n') {
				buf[i] = '\0';
				if (g_str_has_prefix(line, "cpu") && isdigit(line[3])) {
					user = nice = system = idle = id = 0;
					ret = sscanf(line, "%s %ld %ld %ld %ld",
					             cpu, &user, &nice, &system, &idle);
					if (ret != 5) {
						goto next;
					}
					ret = sscanf(cpu, "cpu%d", &id);
					if (ret != 1 || id < 0 || id >= cpu_info.len) {
						goto next;
					}
					user_calc = user - cpu_info.last_user[id];
					nice_calc = nice - cpu_info.last_nice[id];
					system_calc = system - cpu_info.last_system[id];
					idle_calc = idle - cpu_info.last_idle[id];
					total = user_calc + nice_calc + system_calc + idle_calc;
					cpu_info.total[id] = (user_calc + nice_calc + system_calc) / (gfloat)total * 100.;
					cpu_info.last_user[id] = user;
					cpu_info.last_nice[id] = nice;
					cpu_info.last_idle[id] = idle;
					cpu_info.last_system[id] = system;
				}
			  next:
				line = &buf[i + 1];
			}
		}
	}

	g_free(buf);
}

static gpointer G_GNUC_NORETURN
sample_thread (gpointer data)
{
	while (TRUE) {
		g_usleep(G_USEC_PER_SEC);
		next_cpu_info();
	}
}

gint
main (gint   argc,   /* IN */
      gchar *argv[]) /* IN */
{
	GtkWidget *window;
	GtkWidget *cpu;
	GtkWidget *line;
	GtkWidget *map;
	GtkWidget *scatter;
	GdkColor color;
	gint nprocs;
	gint i;
	gint mod;

	gtk_init(&argc, &argv);
	nprocs = get_nprocs();
	/*
	 * Warm up differential samplers.
	 */
	next_cpu_info();
	/*
	 * Install event hook to track how many X events we are doing.
	 */
	gdk_event_handler_set(gdk_event_hook, NULL, NULL);
	/*
	 * Create window and graphs.
	 */
	window = uber_window_new();
	cpu = g_object_new(UBER_TYPE_LINE_GRAPH, NULL);
	line = g_object_new(UBER_TYPE_LINE_GRAPH, NULL);
	map = g_object_new(UBER_TYPE_HEAT_MAP, NULL);
	scatter = g_object_new(UBER_TYPE_SCATTER, NULL);
	/*
	 * Add lines for CPU graph.
	 */
	for (i = 0; i < nprocs; i++) {
		mod = i % G_N_ELEMENTS(default_colors);
		gdk_color_parse(default_colors[mod], &color);
		uber_line_graph_add_line(UBER_LINE_GRAPH(cpu), &color);
	}
	/*
	 * Set data funcs.
	 */
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(cpu),
	                              get_cpu_info, NULL, NULL);
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(line),
	                              get_xevent_info, NULL, NULL);
	uber_line_graph_add_line(UBER_LINE_GRAPH(line), NULL);
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(cpu), "CPU");
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(line), "X Events");
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(map), "IO Latency");
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(scatter), "IOPS By Size");
	gtk_widget_show(scatter);
	gtk_widget_show(map);
	gtk_widget_show(line);
	gtk_widget_show(cpu);
	gtk_widget_show(window);
	g_signal_connect(window,
	                 "delete-event",
	                 G_CALLBACK(gtk_main_quit),
	                 NULL);
	/*
	 * Start sampling thread.
	 */
	g_thread_create(sample_thread, NULL, FALSE, NULL);
	gtk_main();
	return 0;
}
