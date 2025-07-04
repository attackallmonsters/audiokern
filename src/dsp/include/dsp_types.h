#pragma once

#ifdef USE_SINGLE_PRECISION
using dsp_float = float;
#else
using dsp_float = double;
#endif

#ifdef HOST_USE_DOUBLE
using host_float = double;
#else
using host_float = float;
#endif
