#include <gtk/gtk.h>
#include <poppler/glib/poppler.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *drawing_area;
    PopplerDocument *document;
    PopplerPage *current_page;
    int current_page_num;
    double scale;
} PDFViewer;

static void draw_page(GtkWidget *widget, cairo_t *cr, PDFViewer *viewer) {
    if (!viewer->current_page)
        return;

    double page_width, page_height;
    poppler_page_get_size(viewer->current_page, &page_width, &page_height);

    // Scale the page
    cairo_scale(cr, viewer->scale, viewer->scale);

    // Render the page
    poppler_page_render(viewer->current_page, cr);
}

static void next_page(GtkWidget *widget, PDFViewer *viewer) {
    if (!viewer->document)
        return;

    int n_pages = poppler_document_get_n_pages(viewer->document);
    if (viewer->current_page_num < n_pages - 1) {
        viewer->current_page_num++;
        if (viewer->current_page)
            g_object_unref(viewer->current_page);
        viewer->current_page = poppler_document_get_page(viewer->document, viewer->current_page_num);
        gtk_widget_queue_draw(viewer->drawing_area);
    }
}

static void prev_page(GtkWidget *widget, PDFViewer *viewer) {
    if (!viewer->document || viewer->current_page_num <= 0)
        return;

    viewer->current_page_num--;
    if (viewer->current_page)
        g_object_unref(viewer->current_page);
    viewer->current_page = poppler_document_get_page(viewer->document, viewer->current_page_num);
    gtk_widget_queue_draw(viewer->drawing_area);
}

static void zoom_in(GtkWidget *widget, PDFViewer *viewer) {
    viewer->scale *= 1.2;
    gtk_widget_queue_draw(viewer->drawing_area);
}

static void zoom_out(GtkWidget *widget, PDFViewer *viewer) {
    viewer->scale *= 0.8;
    gtk_widget_queue_draw(viewer->drawing_area);
}

static void open_file(GtkWidget *widget, PDFViewer *viewer) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open PDF",
                                                   GTK_WINDOW(viewer->window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                                   "_Open", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *uri = g_filename_to_uri(filename, NULL, NULL);
        
        if (viewer->document)
            g_object_unref(viewer->document);
        
        viewer->document = poppler_document_new_from_file(uri, NULL, NULL);
        viewer->current_page_num = 0;
        viewer->current_page = poppler_document_get_page(viewer->document, 0);
        gtk_widget_queue_draw(viewer->drawing_area);
        
        g_free(filename);
        g_free(uri);
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    PDFViewer viewer = {
        .scale = 1.0,
        .current_page_num = 0,
        .document = NULL,
        .current_page = NULL
    };

    // Create main window
    viewer.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(viewer.window), "PDF Viewer");
    gtk_window_set_default_size(GTK_WINDOW(viewer.window), 800, 600);
    g_signal_connect(viewer.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create vertical box
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(viewer.window), vbox);

    // Create toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    // Add toolbar buttons
    GtkToolItem *open_button = gtk_tool_button_new(NULL, "Open");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_button, -1);
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file), &viewer);

    GtkToolItem *prev_button = gtk_tool_button_new(NULL, "Previous");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), prev_button, -1);
    g_signal_connect(prev_button, "clicked", G_CALLBACK(prev_page), &viewer);

    GtkToolItem *next_button = gtk_tool_button_new(NULL, "Next");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), next_button, -1);
    g_signal_connect(next_button, "clicked", G_CALLBACK(next_page), &viewer);

    GtkToolItem *zoom_in_button = gtk_tool_button_new(NULL, "Zoom In");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), zoom_in_button, -1);
    g_signal_connect(zoom_in_button, "clicked", G_CALLBACK(zoom_in), &viewer);

    GtkToolItem *zoom_out_button = gtk_tool_button_new(NULL, "Zoom Out");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), zoom_out_button, -1);
    g_signal_connect(zoom_out_button, "clicked", G_CALLBACK(zoom_out), &viewer);

    // Create drawing area
    viewer.drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(viewer.drawing_area, 600, 800);
    g_signal_connect(viewer.drawing_area, "draw", G_CALLBACK(draw_page), &viewer);
    
    // Add drawing area to scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), viewer.drawing_area);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    gtk_widget_show_all(viewer.window);
    gtk_main();

    return 0;
}
