/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "actions/create_timeline_selections_action.h"
#include "audio/track.h"
#include "gui/backend/timeline_selections.h"
#include "gui/widgets/center_dock.h"
#include "gui/widgets/timeline_arranger.h"
#include "project.h"
#include "utils/flags.h"
#include "utils/objects.h"

#include <glib/gi18n.h>

/**
 * The given TimelineSelections must already
 * contain the created selections in the transient
 * arrays.
 *
 * Note: chord addresses are to be copied.
 */
UndoableAction *
create_timeline_selections_action_new (
  TimelineSelections * ts)
{
  CreateTimelineSelectionsAction * self =
    calloc (1, sizeof (
                 CreateTimelineSelectionsAction));
  UndoableAction * ua = (UndoableAction *) self;
  ua->type =
    UNDOABLE_ACTION_TYPE_CREATE_TL_SELECTIONS;

  self->ts = timeline_selections_clone (ts);

  return ua;
}

int
create_timeline_selections_action_do (
  CreateTimelineSelectionsAction * self)
{
  Region * region;
	for (int i = 0; i < self->ts->num_regions; i++)
    {
      /* check if the region already exists. due to
       * how the arranger creates regions, the region
       * should already exist the first time so no
       * need to do anything. when redoing we will
       * need to create a clone instead */
      if (region_find (self->ts->regions[i]))
        continue;

      /* clone the transient clone */
      region =
        region_clone (
          self->ts->regions[i],
          REGION_CLONE_COPY);
      g_return_val_if_fail (region->track_pos >= 0,
                            -1);

      /* add it to track */
      track_add_region (
        TRACKLIST->tracks[region->track_pos],
        region);
    }
  /* TODO chords */

  EVENTS_PUSH (ET_TL_SELECTIONS_CHANGED,
               NULL);

  return 0;
}

int
create_timeline_selections_action_undo (
  CreateTimelineSelectionsAction * self)
{
  Region * region;
  for (int i = 0; i < self->ts->num_regions; i++)
    {
      /* get the actual region */
      region = region_find (self->ts->regions[i]);

      /* remove it */
      track_remove_region (
        region->track, region);
      free_later (region, region_free);
    }
  EVENTS_PUSH (ET_TL_SELECTIONS_CHANGED,
               NULL);

  return 0;
}

char *
create_timeline_selections_action_stringize (
  CreateTimelineSelectionsAction * self)
{
  return g_strdup (
    _("Create Object(s)"));
}

void
create_timeline_selections_action_free (
  CreateTimelineSelectionsAction * self)
{
  timeline_selections_free (self->ts);

  free (self);
}
