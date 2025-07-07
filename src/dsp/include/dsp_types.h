#pragma once

#ifdef USE_SINGLE_PRECISION
using dsp_float = float;
#else
using dsp_float = double;
#endif

#ifdef HOST_SINGLE_PRECISION
using host_float = float;
#else
using host_float = double;
#endif
