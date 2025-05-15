
#if 0

/* Variables used under wf-panel */
// gboolean bottom;
// GtkWidget *plugin;
/* Back pointer to the widget */
// PluginGraph graph;
// GdkRGBA foreground_colour;
/* Foreground colour for drawing area */
// GdkRGBA background_colour;
/* Background colour for drawing area */
/* Display usage as a percentage */
// timer for periodic update

//extern void gpm_init(GpuMon *g);
//extern void gpm_update_display(GpuMon *g);
//extern void gpm_free(void *user_data);

/* Handler for system config changed message from panel */
void gpm_update_display (GpuMon *g)
{
    GdkRGBA none = {0, 0, 0, 0};
    graph_reload (&(g->graph),
                  g->icon_size,
                  g->background_colour,
                  g->foreground_colour,
                  none, none);
}

void gpm_init (GpuMon *g)
{
    /* Allocate icon as a child of top level */
    graph_init (&(g->graph));
    gtk_container_add (GTK_CONTAINER (g->plugin), g->graph.da);

    gpm_update_display (g);

    /* Connect a timer to refresh the statistics. */
    g->timer = g_timeout_add (1500, (GSourceFunc) _gpm_update, (gpointer) g);

    /* Show the widget and return. */
    gtk_widget_show_all (g->plugin);
}

void gpm_free (gpointer user_data)
{
    GpuMon *g = (GpuMon *) user_data;
    graph_free (&(g->graph));

    if (g->timer)
        g_source_remove (g->timer);

    g_free (g);
}
#endif

