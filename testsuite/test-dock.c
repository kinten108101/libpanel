#include <libpanel.h>

static PanelFrame *
create_frame_cb (PanelGrid *grid)
{
  PanelFrame *frame = PANEL_FRAME (panel_frame_new ());
  PanelFrameHeader *header = PANEL_FRAME_HEADER (panel_frame_tab_bar_new ());

  g_print ("Create frame\n");

  panel_frame_tab_bar_set_start_child (PANEL_FRAME_TAB_BAR (header), gtk_label_new ("Start Child"));
  panel_frame_tab_bar_set_end_child (PANEL_FRAME_TAB_BAR (header), gtk_label_new ("End Child"));
  panel_frame_set_header (frame, header);

  return frame;
}

static PanelWidget *
create_document (void)
{
  static guint count;
  char *title = g_strdup_printf ("Untitled Document %u", ++count);
  PanelWidget *ret;

  ret = g_object_new (PANEL_TYPE_WIDGET,
                      "title", title,
                      "icon-name", "text-x-document-symbolic",
                      "child", g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                             "child", g_object_new (GTK_TYPE_TEXT_VIEW,
                                                                    "buffer", g_object_new (GTK_TYPE_TEXT_BUFFER,
                                                                                            "text", "Hello libpanel!",
                                                                                            NULL),
                                                                    NULL),
                                             NULL),
                      NULL);

  g_free (title);

  return ret;
}

static void
add_child (PanelGrid *grid)
{
  PanelWidget *widget = create_document ();
  panel_grid_add (grid, widget);
}

static void
add_clicked_cb (PanelGrid *grid,
                GtkButton *button)
{
  g_assert (GTK_IS_BUTTON (button));
  g_assert (PANEL_IS_GRID (grid));

  add_child (grid);
}

int
main (int argc,
      char *argv[])
{
  GMainLoop *main_loop;
  GtkBuilder *builder;
  GtkWindow *window;
  g_autofree char *filename = NULL;
  GtkBuilderScope *scope;
  GError *error = NULL;

  gtk_init ();
  panel_init ();

  main_loop = g_main_loop_new (NULL, FALSE);
  filename = g_build_filename (g_getenv ("G_TEST_SRCDIR"), "test-dock.ui", NULL);
  builder = gtk_builder_new ();
  scope = gtk_builder_get_scope (builder);
  gtk_builder_cscope_add_callback_symbol (GTK_BUILDER_CSCOPE (scope), "create_frame_cb", G_CALLBACK (create_frame_cb));
  gtk_builder_cscope_add_callback_symbol (GTK_BUILDER_CSCOPE (scope), "add_clicked_cb", G_CALLBACK (add_clicked_cb));
  gtk_builder_add_from_file (builder, filename, &error);
  g_assert_no_error (error);

  add_child (PANEL_GRID (gtk_builder_get_object (builder, "grid")));

  window = GTK_WINDOW (gtk_builder_get_object (builder, "window"));
  g_signal_connect_swapped (window, "close-request", G_CALLBACK (g_main_loop_quit), main_loop);
  gtk_window_present (window);
  g_main_loop_run (main_loop);

  return 0;
}
