#pragma once

#include "../utils/utils.h"

gboolean on_configure_window(GtkWindow *window, GdkEvent *event, gpointer user_data);

gboolean on_save_button(GtkWidget *widget, gpointer user_data);
gboolean on_open_button(GtkWidget *widget, gpointer user_data);

gboolean on_nn_generate_btn(GtkWidget* widget, gpointer user_data);
gboolean on_nn_load_file_set(GtkWidget* widget, gpointer user_data);
gboolean on_nn_load_btn(GtkWidget* widget, gpointer user_data);
gboolean on_nn_export_btn(GtkWidget* widget, gpointer user_data);

gboolean on_train_file_set(GtkWidget* widget, gpointer user_data);
gboolean on_epochs_spin_btn(GtkWidget* widget, gpointer user_data);
gboolean on_lr_spin_btn(GtkWidget* widget, gpointer user_data);
