/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
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
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 *
 * Audio clip.
 */

#ifndef __AUDIO_CLIP_H__
#define __AUDIO_CLIP_H__

#include "utils/types.h"

/**
 * @addtogroup audio
 *
 * @{
 */

/**
 * Audio clips for the pool.
 *
 * These should be loaded in the project's sample
 * rate.
 */
typedef struct AudioClip
{
  /** Name of the clip. */
  char *        name;

  /** The audio frames. */
  sample_t *    frames;

  /** Number of frames per channel. */
  long          num_frames;

  /** Number of channels. */
  channels_t    channels;

  /** ID (index) in the audio pool. */
  int           pool_id;
} AudioClip;

/**
 * Creates an audio clip from a file.
 *
 * The name used is the basename of the file.
 */
AudioClip *
audio_clip_new_from_file (
  const char * full_path);

/**
 * Creates an audio clip by copying the given float
 * array.
 *
 * @param name A name for this clip.
 */
AudioClip *
audio_clip_new_from_float_array (
  const float *    arr,
  const long       nframes,
  const channels_t channels,
  const char *     name);

/**
 * Writes the given audio clip data to a file.
 */
void
audio_clip_write_to_file (
  const AudioClip * self,
  const char *      filepath);

/**
 * Writes the clip to the pool as a wav file.
 */
void
audio_clip_write_to_pool (
  const AudioClip * self);

/**
 * @}
 */

#endif