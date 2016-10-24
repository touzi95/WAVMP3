#include "lame_encoder.h"
#include "lame_encoder_exception.h"

#include <lame.h>

namespace wav2mp3 {

lame_encoder::lame_encoder(int samples_per_second, encoding_quality quality)
{
  if (lame_set_in_samplerate(state_, samples_per_second) != 0)
    throw lame_encoder_exception{ "Unable to init encoder samplerate" };

  if (lame_set_num_channels(state_, 2) != 0)
    throw lame_encoder_exception{ "Unable to init encoder channels" };

  if (lame_set_quality(state_, static_cast<int>(quality)) != 0)
    throw lame_encoder_exception{ "Unable to init encoder quality" };

  if (lame_init_params(state_) != 0)
    throw lame_encoder_exception{ "Unable to init encoder quality" };
}

std::vector<unsigned char>
lame_encoder::process(std::vector<pcm::sample> samples)
{
  std::vector<unsigned char> buffer;
  buffer.resize(samples.size() * 5 / 4 + 7200);

  auto frame_bytes = lame_encode_buffer_interleaved(
    state_, &(samples.data()->left), static_cast<int>(samples.size()),
    buffer.data(), static_cast<int>(buffer.size()));

  if (frame_bytes < 0)
    return {};

  std::vector<unsigned char> frames{ buffer.data(),
                                     buffer.data() + frame_bytes };

  frame_bytes =
    lame_encode_flush(state_, buffer.data(), static_cast<int>(buffer.size()));

  if (frame_bytes > 0)
    frames.insert(frames.end(), buffer.data(), buffer.data() + frame_bytes);

  return frames;
}

lame_encoder::state::state()
  : flags_{ lame_init() }
{
  if (!flags_)
    throw lame_encoder_exception{ "Unable to init lame encoder" };
}
lame_encoder::state::~state()
{
  lame_close(flags_);
}
}
