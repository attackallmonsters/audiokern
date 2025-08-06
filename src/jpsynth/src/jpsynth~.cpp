// jpsynth.cpp - Pure Data external wrapping the Voice audio synthesis class

#include "m_pd.h"
#include "DSP.h"
#include "JPVoice.h"
#include "JPSynth.h"
#include "clamp.h"
#include "dsp_types.h"

#define synth JPSynth::instance()

static t_class *jpsynth_class;

typedef struct _jpsynth
{
    t_object x_obj;

    t_inlet *in_cutoff;
    t_inlet *in_reso;
    t_outlet *left_out;
    t_outlet *right_out;
    host_float left;
    host_float right;
    dsp_float samplerate;
    size_t blockSize;
    LFOParams lfo1;
    LFOParams lfo2;
} t_jpsynth;

bool testDSP()
{
    if (!DSP::isInitialized())
    {
        post("%s", "DSP not active");
        return false;
    }

    return true;
}

// Frequency of carrier set via list [f1 freq(
void jpsynth_tilde_note(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 2)
    {
        pd_error(x, "[jpsynth~]: expected float arguments 0 - n for note and velocity: [note f v(");
        return;
    }

    host_float n = atom_getfloat(&argv[0]);
    host_float v = atom_getfloat(&argv[1]);

    synth.noteIn(n, v);
}

// Frequency offset modulator in halftones
void jpsynth_tilde_offset(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected float argument -24 - 24 for modulator offset: [offset f(");
        return;
    }

    host_float offset = atom_getfloat(argv);
    synth.setPitchOffset(offset);
}

// Frequency fine tuning for modulator -100 - 100 [fine f(
void jpsynth_tilde_fine(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected float argument -100 - 100 for modulator fine tune: [fine f(");
        return;
    }

    host_float finetune = clamp(atom_getfloat(argv), -100.0f, 100.0f);
    synth.setFineTune(finetune);
}

// Frequency of modulator set via list [detune factor(
void jpsynth_tilde_detune(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected float argument 0 - 1 for detune factor: [detune f(");
        return;
    }

    host_float detune = atom_getfloat(argv);
    synth.setDetune(detune);
}

// Oscillator type carrier [carrier n( 1 - 5
void jpsynth_tilde_carrier(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 1 - 5 for carrier oscillator type: [carrier n(");
        return;
    }

    switch (atom_getint(argv))
    {
    case 1:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Saw);
        break;
    case 2:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Square);
        break;
    case 3:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Triangle);
        break;
    case 4:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Sine);
        break;
    case 5:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Cluster);
        break;
    case 6:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Fibonacci);
        break;
    case 7:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Mirror);
        break;
    case 8:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Modulo);
        break;
    default:
        synth.setCarrierOscillatorType(CarrierOscillatiorType::Saw);
        break;
    }
}

// Oscillator type carrier [modulator n( 1 - 4
void jpsynth_tilde_modulator(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 1 - 4 for modulation oscillator type: [modulator n(");
        return;
    }

    switch (atom_getint(argv))
    {
    case 1:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Saw);
        break;
    case 2:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Square);
        break;
    case 3:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Triangle);
        break;
    case 4:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Sine);
        break;
    case 5:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Cluster);
        break;
    case 6:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Fibonacci);
        break;
    case 7:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Mirror);
        break;
    case 8:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Modulo);
        break;
    case 9:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Bit);
        break;
    default:
        synth.setModulatorOscillatorType(ModulatorOscillatorType::Sine);
        break;
    }
}

// Sets the type of noise to white (0) or pink (1)
void jpsynth_tilde_noisetype(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 0 (white) or 1 (pink) for noise type: [noisetype n(");
        return;
    }

    switch (atom_getint(argv))
    {
    case 0:
        synth.setNoiseType(NoiseType::White);
        break;
    case 1:
        synth.setNoiseType(NoiseType::Pink);
        break;
    default:
        synth.setNoiseType(NoiseType::White);
        break;
    }
}

// Oscillator mix [oscmix f(
void jpsynth_tilde_oscmix(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 0 - 1 for oscillator mix: [oscmix f(");
        return;
    }

    float mix = atom_getfloat(argv);
    synth.setOscillatorMix(mix);
}

// Noise mix [noisemix f(
void jpsynth_tilde_noisemix(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 0 - 1 for noise mix: [noisemix f(");
        return;
    }

    float mix = atom_getfloat(argv);
    synth.setNoiseMix(mix);
}

// Sets the FM modulation index [fmmod f(
void jpsynth_tilde_modidx(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 0 - n for FM/PM modulation index: [modidx f(");
        return;
    }

    host_float idx = atom_getfloat(argv);
    synth.setModulation(idx);
}

// Sets the pitch bend [bend f(
void jpsynth_tilde_bend(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 0 - n for FM/PM modulation index: [modidx f(");
        return;
    }

    host_float bend = atom_getfloat(argv);
    synth.setPitchBend(bend);
}

// Sets the number of voices 1 - 9 [nov f(
void jpsynth_tilde_nov(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 1 - 9 for number of voices: [nov f(");
        return;
    }

    int nov = atom_getint(argv);
    synth.setNumVoices(nov);
}

// Oscillator sync
void jpsynth_tilde_sync(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpsynth~]: expected int argument 0 (unsynced) or 1 (synced) for oscillator sync: [oscsync n(");
        return;
    }

    int enabled = atom_getfloat(argv);
    synth.setSyncEnabled(enabled != 0);
}

// [filtermode <0|1|2>] → 0 = LPF12, 1 = BPF12, 2 = HPF12
void jpsynth_tilde_mode(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: mode <0=LP | 1=HP>");
        return;
    }

    int mode = atom_getint(argv);

    switch (mode)
    {
    case 0:
        synth.setFilterMode(FilterMode::LP);
        break;
    case 1:
        synth.setFilterMode(FilterMode::HP);
        break;
    default:
        synth.setFilterMode(FilterMode::LP);
        break;
    }
}

void jpsynth_tilde_follow(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: follow 0|1");
        return;
    }

    int f = atom_getfloat(argv);

    synth.setFilterFollow(f != 0);
}

void jpsynth_tilde_drift(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 2)
    {
        post("[jpsynth~] usage: drift amount 0 - 1, damping 0 - 1");
        return;
    }

    host_float a = atom_getfloat(&argv[0]);
    host_float d = atom_getfloat(&argv[1]);

    synth.setAnalogDrift(a, d);
}

// [carrierfb (0 - 1.2)]
void jpsynth_tilde_carrierfb(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: carrierfb (amount 0.0 – 1.2)");
        return;
    }

    host_float fb = atom_getfloat(argv);
    synth.setFeedbackCarrier(fb);
}

// [carrierfb (0 - 1.2)]
void jpsynth_tilde_modulatorfb(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: modulatorfb (amount 0.0 – 1.2)");
        return;
    }

    host_float fb = atom_getfloat(argv);
    synth.setFeedbackModulator(fb);
}

void jpsynth_tilde_cutoff(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: filter cutoff (amount 0.0 – samplerate / 2)");
        return;
    }

    host_float f = atom_getfloat(argv);
    synth.setFilterCutoff(f);
}

void jpsynth_tilde_reso(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: filter reso (amount 0.0 – 1.0)");
        return;
    }

    host_float r = atom_getfloat(argv);
    synth.setFilterResonance(r);
}

void jpsynth_tilde_drive(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: filter drive (amount 0.0 - 1.0)");
        return;
    }

    host_float d = atom_getfloat(argv);
    synth.setFilterDrive(d);
}

// Filter ADSR [fltadsr att dec sus rel cutoff attshape relshape(
void jpsynth_tilde_fltadsr(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpsynth~]: expected filter ADSR settings [fltadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    ADSRParams adsr;

    adsr.attackTime = atom_getfloat(&argv[0]);
    adsr.decayTime = atom_getfloat(&argv[1]);
    adsr.sustainLevel = atom_getfloat(&argv[2]);
    adsr.releaseTime = atom_getfloat(&argv[3]);
    adsr.attackShape = atom_getfloat(&argv[4]);
    adsr.releaseShape = atom_getfloat(&argv[5]);

    synth.setFilterADSR(adsr);
}

// Filter ADSR [ampadsr att dec sus rel cutoff attshape relshape(
void jpsynth_tilde_ampadsr(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpsynth~]: expected amp ADSR settings [ampadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    ADSRParams adsr;

    adsr.attackTime = atom_getfloat(&argv[0]);
    adsr.decayTime = atom_getfloat(&argv[1]);
    adsr.sustainLevel = atom_getfloat(&argv[2]);
    adsr.releaseTime = atom_getfloat(&argv[3]);
    adsr.attackShape = atom_getfloat(&argv[4]);
    adsr.releaseShape = atom_getfloat(&argv[5]);

    synth.setAmpADSR(adsr);
}

void jpsynth_tilde_adsrlink(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: adsrlink [adsrlink 0|1(");
        return;
    }

    synth.linkADSR(atom_getfloat(argv) != 0.0);
}

void jpsynth_tilde_adsroneshot(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: adsroneshot [adsroneshot 0|1(");
        return;
    }

    synth.setADSROneshot(atom_getfloat(argv) != 0.0);
}

// LFO1 [lfo1 type freq offset depth shape pw smooth target(
void jpsynth_tilde_lfo1(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 8)
    {
        pd_error(x, "[jpsynth~]: expected LFO1 settings [lfo1 freq type offset depth shape pw smooth target(");
        return;
    }

    LFOParams lfo;

    lfo.frequency = atom_getfloat(&argv[0]);

    switch (atom_getint(&argv[1]))
    {
    case 0:
        lfo.type = LFOType::Sine;
        break;
    case 1:
        lfo.type = LFOType::RampUp;
        break;
    case 2:
        lfo.type = LFOType::RampDown;
        break;
    case 3:
        lfo.type = LFOType::Triangle;
        break;
    case 4:
        lfo.type = LFOType::Square;
        break;
    case 5:
        lfo.type = LFOType::Random;
        break;
    default:
        lfo.type = LFOType::Sine;
        break;
    }

    lfo.offset = atom_getfloat(&argv[2]);
    lfo.depth = atom_getfloat(&argv[3]);
    lfo.shape = atom_getfloat(&argv[4]);
    lfo.pw = atom_getfloat(&argv[5]);
    lfo.smooth = atom_getfloat(&argv[6]);

    switch (atom_getint(&argv[7]))
    {
    case 0:
        lfo.target = LFOTarget::None;
        break;
    case 1:
        lfo.target = LFOTarget::Cutoff;
        break;
    case 2:
        lfo.target = LFOTarget::Tremolo;
        break;
    case 3:
        lfo.target = LFOTarget::Vibrato;
        break;
    case 4:
        lfo.target = LFOTarget::Panning;
        break;
    case 5:
        lfo.target = LFOTarget::OscMix;
        break;
    default:
        lfo.target = LFOTarget::None;
        break;
    }

    synth.setLFO1(lfo);
}

// LFO2 [lfo2 type freq offset depth shape pw smooth target(
void jpsynth_tilde_lfo2(t_jpsynth *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpsynth~]: expected filter ADSR settings [fltadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    
}

void jpsynth_tilde_revroom(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: revroom (amount 0.0 - 1.0)");
        return;
    }

    host_float room = atom_getfloat(argv);
    synth.setReverbRoom(room);
}

void jpsynth_tilde_revspace(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: revspace (amount 0.0 - 1.0)");
        return;
    }

    host_float space = atom_getfloat(argv);
    synth.setReverbSpace(space);
}

void jpsynth_tilde_revdamp(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: revdamp (amount 0.0 - 10000.0)");
        return;
    }

    host_float damping = atom_getfloat(argv);
    synth.setReverbDamping(damping);
}

void jpsynth_tilde_revdense(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: revdense (amount 0.0 - 1,0)");
        return;
    }

    host_float density = atom_getfloat(argv);
    synth.setReverbDensity(density);
}

void jpsynth_tilde_revdisp(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: revdisp (amount 0.0 - 1.0)");
        return;
    }

    host_float fdisp = clamp(atom_getfloat(argv), 0.0, 1.0);
    int disp = dsp_math::normf_to_int_range(fdisp, 0, 16);

    switch (disp)
    {
    case 0:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::NONE);
        break;
    case 1:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::HALF);
        break;
    case 2:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::DOUBLE);
        break;
    case 3:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::TRIPLET);
        break;
    case 4:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::DOTTED);
        break;
    case 5:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::POLY_3_4);
        break;
    case 6:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::POLY_4_3);
        break;
    case 7:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::POLY_3_5);
        break;
    case 8:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::POLY_5_3);
        break;
    case 9:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::GOLDEN_RATIO);
        break;
    case 10:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::SILVER_RATIO);
        break;
    case 11:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::PLATINUM_RATIO);
        break;
    case 12:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::SQRT_2);
        break;
    case 13:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::SQRT_3);
        break;
    case 14:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::PHI_INV);
        break;
    case 15:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::PI_REL);
        break;
    case 16:
        synth.setReverbTimeRatio(dsp_math::TimeRatio::E_REL);
        break;
    default:
        break;
    }
}

void jpsynth_tilde_revwet(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: revvol (revvol 0.0 - 1,0)");
        return;
    }

    host_float v = atom_getfloat(argv);
    synth.setReverbWet(v);
}

void jpsynth_tilde_deltime(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: deltime (deltime tL tR)");
        return;
    }

    host_float tL = atom_getfloat(&argv[0]);
    host_float tR = atom_getfloat(&argv[1]);

    synth.setDelayTime(tL, tR);
}

void jpsynth_tilde_delfb(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: delfb (deltime fbL fbR)");
        return;
    }

    host_float fbL = atom_getfloat(&argv[0]);
    host_float fbR = atom_getfloat(&argv[1]);

    synth.setDelayFeedback(fbL, fbR);
}

void jpsynth_tilde_deldisp(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: deldisp (deldisp 0.0 - 1.0)");
        return;
    }

    host_float fdisp = clamp(atom_getfloat(argv), 0.0, 1.0);
    int disp = dsp_math::normf_to_int_range(fdisp, 0, 16);

    switch (disp)
    {
    case 0:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::NONE);
        break;
    case 1:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::HALF);
        break;
    case 2:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::DOUBLE);
        break;
    case 3:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::TRIPLET);
        break;
    case 4:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::DOTTED);
        break;
    case 5:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::POLY_3_4);
        break;
    case 6:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::POLY_4_3);
        break;
    case 7:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::POLY_3_5);
        break;
    case 8:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::POLY_5_3);
        break;
    case 9:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::GOLDEN_RATIO);
        break;
    case 10:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::SILVER_RATIO);
        break;
    case 11:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::PLATINUM_RATIO);
        break;
    case 12:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::SQRT_2);
        break;
    case 13:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::SQRT_3);
        break;
    case 14:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::PHI_INV);
        break;
    case 15:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::PI_REL);
        break;
    case 16:
        synth.setDelayTimeRatio(dsp_math::TimeRatio::E_REL);
        break;
    default:
        break;
    }
}

void jpsynth_tilde_delwet(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: delwet (delwet 0.0 - 1.0)");
        return;
    }

    host_float v = atom_getfloat(argv);
    synth.setDelayWet(v);
}

void jpsynth_tilde_distwet(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: distwet (delwet 0.0 - 1.0)");
        return;
    }

    synth.setDistWet(atom_getfloat(argv));
}

void jpsynth_tilde_distdrive(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: distdrive (deldrive 0.0 - 1.0)");
        return;
    }

    synth.setDistDrive(atom_getfloat(argv));
}

void jpsynth_tilde_distgain(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: distgain (delgain 0.0 - 1.0)");
        return;
    }

    synth.setDistGain(atom_getfloat(argv));
}

void jpsynth_tilde_disttone(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: disttone (deltone 0.0 - 1.0)");
        return;
    }

    synth.setDistTone(atom_getfloat(argv));
}

void jpsynth_tilde_disttype(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: disttype (deltype 0.0 - 1.0)");
        return;
    }

    int distType = atom_getint(argv);

    switch (distType)
    {
    case 0:
        synth.setDistType(Distortion::DistortionType::SoftClip);
        break;
    case 1:
        synth.setDistType(Distortion::DistortionType::HardClip);
        break;
    case 2:
        synth.setDistType(Distortion::DistortionType::Foldback);
        break;
    case 3:
        synth.setDistType(Distortion::DistortionType::Tube);
        break;    
    default:
        synth.setDistType(Distortion::DistortionType::SoftClip);
        break;
    }
}

void jpsynth_tilde_wet(t_jpsynth * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpsynth~] usage: wet (wet 0.0 - 1,0)");
        return;
    }

    host_float w = atom_getfloat(argv);
    synth.setWet(w);
}

// DSP perform function
t_int *jpsynth_tilde_perform(t_int *w)
{
    synth.process();
    //DSPBusManager::validate();
    return (w + 5);
}

// DSP add function
void jpsynth_tilde_dsp(t_jpsynth *x, t_signal **sp)
{
    t_sample *outL = sp[0]->s_vec;
    t_sample *outR = sp[1]->s_vec;

    x->samplerate = sp[0]->s_sr;
    x->blockSize = sp[0]->s_n;

    DSP::initializeAudio(x->samplerate, x->blockSize);

    synth.initialize(outL, outR);

    DSPBusManager::log();

    dsp_add(jpsynth_tilde_perform, 4,
            x,
            sp[0]->s_vec, // outL
            sp[1]->s_vec, // outR
            sp[0]->s_n);  // Block size
}

void log(const std::string &entry)
{
    post("%s", entry.c_str());
}

// Constructor
void *jpsynth_tilde_new()
{
    t_jpsynth *x = (t_jpsynth *)pd_new(jpsynth_class);

    x->left_out = outlet_new(&x->x_obj, &s_signal);
    x->right_out = outlet_new(&x->x_obj, &s_signal);

    DSP::registerLogger(&log);

    return (void *)x;
}

// Destructor
void jpsynth_tilde_free(t_jpsynth *x)
{
    outlet_free(x->left_out);
    outlet_free(x->right_out);

    // delete x->voice;
}

// Setup function
extern "C" void jpsynth_tilde_setup(void)
{
    jpsynth_class = class_new(gensym("jpsynth~"),
                              (t_newmethod)jpsynth_tilde_new,
                              (t_method)jpsynth_tilde_free,
                              sizeof(t_jpsynth),
                              CLASS_DEFAULT,
                              A_NULL);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_dsp, gensym("dsp"), A_CANT, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_note, gensym("note"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_offset, gensym("offset"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_fine, gensym("fine"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_detune, gensym("detune"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_carrier, gensym("carrier"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_modulator, gensym("modulator"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_oscmix, gensym("oscmix"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_noisemix, gensym("noisemix"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_noisetype, gensym("noisetype"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_modidx, gensym("modidx"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_bend, gensym("bend"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_nov, gensym("nov"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_sync, gensym("sync"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_carrierfb, gensym("carrierfb"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_modulatorfb, gensym("modulatorfb"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_cutoff, gensym("cutoff"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_reso, gensym("reso"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_drive, gensym("drive"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_mode, gensym("mode"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_follow, gensym("follow"), A_GIMME, 0);

    // TODO
    // class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_drift, gensym("drift"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_fltadsr, gensym("fltadsr"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_ampadsr, gensym("ampadsr"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_adsrlink, gensym("adsrlink"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_adsroneshot, gensym("adsroneshot"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_lfo1, gensym("lfo1"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_lfo2, gensym("lfo2"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_revroom, gensym("revroom"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_revspace, gensym("revspace"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_revdamp, gensym("revdamp"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_revdense, gensym("revdense"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_revdisp, gensym("revdisp"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_revwet, gensym("revwet"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_deltime, gensym("deltime"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_delfb, gensym("delfb"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_deldisp, gensym("deldisp"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_delwet, gensym("delwet"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_distwet, gensym("distwet"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_disttype, gensym("disttype"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_distdrive, gensym("distdrive"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_distgain, gensym("distgain"), A_GIMME, 0);
    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_disttone, gensym("disttone"), A_GIMME, 0);

    class_addmethod(jpsynth_class, (t_method)jpsynth_tilde_wet, gensym("wet"), A_GIMME, 0);
}
