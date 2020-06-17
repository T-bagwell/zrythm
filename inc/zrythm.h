/*
 * Copyright (C) 2019-2020 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 *
 * The Zrythm GTK application.
 */

#ifndef __ZRYTHM_H__
#define __ZRYTHM_H__

#include "audio/metronome.h"
#include "audio/snap_grid.h"
#include "gui/backend/file_manager.h"
#include "plugins/plugin_manager.h"
#include "settings/settings.h"
#include "utils/log.h"

#include "zix/sem.h"

#include <gtk/gtk.h>

typedef struct _MainWindowWidget MainWindowWidget;
typedef struct Project Project;
typedef struct Symap Symap;
typedef struct CairoCaches CairoCaches;
typedef struct UiCaches UiCaches;
typedef struct MPMCQueue MPMCQueue;
typedef struct ObjectPool ObjectPool;
typedef struct RecordingManager RecordingManager;
typedef struct EventManager EventManager;
typedef struct ObjectUtils ObjectUtils;

/**
 * @addtogroup general
 *
 * @{
 */

#define ZRYTHM (zrythm)

#define ZRYTHM_PROJECTS_DIR "Projects"

#define MAX_RECENT_PROJECTS 20
#define DEBUGGING (ZRYTHM->debug)
#define ZRYTHM_TESTING (ZRYTHM->testing)
#define ZRYTHM_HAVE_UI (ZRYTHM->have_ui)

/**
 * Type of Zrythm directory.
 */
typedef enum ZrythmDirType
{
  /*
   * ----
   * System directories that ship with Zrythm
   * and must not be changed.
   * ----
   */

  /**
   * The prefix, or in the case of windows installer
   * the root dir (C/program files/zrythm), or in the
   * case of macos installer the bundle path.
   *
   * In all cases, "share" is expected to be found
   * in this dir.
   */
  ZRYTHM_DIR_SYSTEM_PREFIX,

  /** "bin" under \ref ZRYTHM_DIR_SYSTEM_PREFIX. */
  ZRYTHM_DIR_SYSTEM_BINDIR,

  /** "share" under \ref ZRYTHM_DIR_SYSTEM_PREFIX. */
  ZRYTHM_DIR_SYSTEM_PARENT_DATADIR,

  /** Localization under "share". */
  ZRYTHM_DIR_SYSTEM_LOCALEDIR,

  /** share/zrythm */
  ZRYTHM_DIR_SYSTEM_ZRYTHM_DATADIR,

  /** Samples. */
  ZRYTHM_DIR_SYSTEM_SAMPLESDIR,

  /** Themes. */
  ZRYTHM_DIR_SYSTEM_THEMESDIR,

  /* ************************************ */

  /*
   * ----
   * Zrythm user directories that contain
   * user-modifiable data.
   * ----
   */

  /** Main zrythm directory from gsettings. */
  ZRYTHM_DIR_USER_TOP,

  /** Subdirs of \ref ZRYTHM_DIR_USER_TOP. */
  ZRYTHM_DIR_USER_PROJECTS,
  ZRYTHM_DIR_USER_TEMPLATES,
  ZRYTHM_DIR_USER_LOG,
  ZRYTHM_DIR_USER_THEMES,

} ZrythmDirType;

/**
 * To be used throughout the program.
 *
 * Everything here should be global and function regardless
 * of the project.
 */
typedef struct Zrythm
{
  /**
   * Manages plugins (loading, instantiating, etc.)
   */
  PluginManager       plugin_manager;

  /** Main window. */
  MainWindowWidget *  main_window;

  /**
   * Application settings
   */
  Settings            settings;

  /**
   * Project data.
   *
   * This is what should be exported/imported when
   * saving/loading projects.
   *
   * The only reason this is a pointer is to easily
   * deserialize.
   */
  Project *           project;

  /** +1 to ensure last element is NULL in case
   * full. */
  char *
    recent_projects[MAX_RECENT_PROJECTS + 1];
  int                 num_recent_projects;

  /** NULL terminated array of project template
   * absolute paths. */
  char **             templates;

  /** The metronome. */
  Metronome           metronome;

  /** Whether the open file is a template to be used
   * to create a new project from. */
  bool                opening_template;

  /** Whether creating a new project, either from
   * a template or blank. */
  bool                creating_project;

  /** Path to create a project in, including its
   * title. */
  char *              create_project_path;

  /**
   * Filename to open passed through the command
   * line.
   *
   * Used only when a filename is passed.
   * E.g., zrytm myproject.xml
   */
  char *              open_filename;

  EventManager *      event_manager;

  /** Recording manager. */
  RecordingManager *  recording_manager;

  /** File manager. */
  FileManager         file_manager;

  /**
   * String interner for internal things.
   */
  Symap *             symap;

  CairoCaches *       cairo_caches;

  UiCaches *          ui_caches;

  /** Object utils. */
  ObjectUtils *       object_utils;

  /**
   * In debug mode or not (determined by GSetting).
   */
  bool                debug;

  /**
   * Used when running the tests.
   *
   * This is set by the TESTING environment variable.
   */
  bool                testing;

  /** Initialization thread. */
  GThread *           init_thread;

  /**
   * The GTK thread where the main GUI loop runs.
   *
   * This is stored for identification purposes
   * in other threads.
   */
  GThread *           gtk_thread;

  /** Status text to be used in the splash screen. */
  char                status[800];

  /** Semaphore for setting the progress in the
   * splash screen from a non-gtk thread. */
  ZixSem              progress_status_lock;

  /** Log settings FIXME allocate. */
  Log                 log;

  /** Flag to set when initialization has
   * finished. */
  bool                init_finished;

  /**
   * Progress done (0.0 ~ 1.0).
   *
   * To be used in things like the splash screen,
   * loading projects, etc.
   */
  double              progress;

  /** 1 if Zrythm has a UI, 0 if headless (eg, when
   * unit-testing). */
  bool                have_ui;
} Zrythm;

/**
 * Global variable, should be available to all files.
 */
extern Zrythm * zrythm;

void
zrythm_add_to_recent_projects (
  Zrythm * self,
  const char * filepath);

void
zrythm_remove_recent_project (
  char * filepath);

/**
 * Returns the version string.
 *
 * Must be g_free()'d.
 *
 * @param with_v Include a starting "v".
 */
char *
zrythm_get_version (
  int with_v);

/**
 * Returns the veresion and the capabilities.
 */
void
zrythm_get_version_with_capabilities (
  char * str);

/**
 * Returns the default user "zrythm" dir.
 *
 * This is used when resetting or when the
 * dir is not selected by the user yet.
 */
char *
zrythm_get_default_user_dir (void);

/**
 * Returns a Zrythm directory specified by
 * \ref type.
 *
 * @return A newly allocated string.
 */
char *
zrythm_get_dir (
  ZrythmDirType type);

/**
 * Sets the current status and progress percentage
 * during loading.
 *
 * The splash screen then reads these values from
 * the Zrythm struct.
 */
void
zrythm_set_progress_status (
  Zrythm *     self,
  const char * text,
  const double perc);

/**
 * Returns the prefix or in the case of windows
 * the root dir (C/program files/zrythm) or in the
 * case of macos the bundle path.
 *
 * In all cases, "share" is expected to be found
 * in this dir.
 *
 * @return A newly allocated string.
 */
char *
zrythm_get_prefix (void);

/**
 * Gets the zrythm directory, either from the
 * settings if non-empty, or the default
 * ($XDG_DATA_DIR/zrythm).
 *
 * @param force_default Ignore the settings and get
 *   the default dir.
 *
 * Must be free'd by caler.
 */
char *
zrythm_get_user_dir (
  bool  force_default);

/**
 * Creates a new Zrythm instance.
 *
 * @param have_ui Whether Zrythm is instantiated
 *   with a UI (false if headless).
 * @param testing Whether this is a unit test.
 */
Zrythm *
zrythm_new (
  bool have_ui,
  bool testing);

/**
 * Frees the instance and any unfreed members.
 */
void
zrythm_free (
  Zrythm * self);

/**
 * @}
 */

#endif /* __ZRYTHM_H__ */
