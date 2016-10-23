#include "mp3.h"
#include "lame.h"

#include <ostream>

namespace wav2mp3 {

mp3::mp3(pcm const& source)
  : frames_{ lame_encoder{ source.samples_per_second(), source.channels(), 5 }.process(source.samples()) }
{
}

std::ostream&
operator<<(std::ostream& ostr, mp3 const& data)
{
  return ostr.write(reinterpret_cast<char const*>(data.frames_.data()),
                    data.frames_.size());
}
}
