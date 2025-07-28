// jpvoice.cpp - Pure Data external wrapping the Voice audio synthesis class

#include "m_pd.h"
#include "DSP.h"
#include "JPVoice.h"
#include "JPSynth.h"
#include "clamp.h"
#include "dsp_types.h"

#define synth JPSynth::instance()

static t_class *jpvoice_class;

typedef struct _jpvoice
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
    ADSRParams filterADSR;
    ADSRParams ampADSR;
    LFOParams lfo1;
    LFOParams lfo2;
} t_jpvoice;

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
void jpvoice_tilde_note(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 2)
    {
        pd_error(x, "[jpvoice~]: expected float arguments 0 - n for note and velocity: [note f v(");
        return;
    }

    host_float n = atom_getfloat(&argv[0]);
    host_float v = atom_getfloat(&argv[1]);

    synth.noteIn(n, v);
}

// Frequency offset modulator in halftones
void jpvoice_tilde_offset(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument -24 - 24 for modulator offset: [offset f(");
        return;
    }

    host_float offset = atom_getfloat(argv);
    synth.setPitchOffset(offset);
}

// Frequency fine tuning for modulator -100 - 100 [fine f(
void jpvoice_tilde_fine(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument -100 - 100 for modulator fine tune: [fine f(");
        return;
    }

    host_float finetune = clamp(atom_getfloat(argv), -100.0f, 100.0f);
    synth.setFineTune(finetune);
}

// Frequency of modulator set via list [detune factor(
void jpvoice_tilde_detune(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - 1 for detune factor: [detune f(");
        return;
    }

    host_float detune = atom_getfloat(argv);
    synth.setDetune(detune);
}

// Oscillator type carrier [carrier n( 1 - 5
void jpvoice_tilde_carrier(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 5 for carrier oscillator type: [carrier n(");
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
void jpvoice_tilde_modulator(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 4 for modulation oscillator type: [modulator n(");
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
void jpvoice_tilde_noisetype(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 (white) or 1 (pink) for noise type: [noisetype n(");
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
void jpvoice_tilde_oscmix(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - 1 for oscillator mix: [oscmix f(");
        return;
    }

    float mix = atom_getfloat(argv);
    synth.setOscillatorMix(mix);
}

// Noise mix [noisemix f(
void jpvoice_tilde_noisemix(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - 1 for noise mix: [noisemix f(");
        return;
    }

    float mix = atom_getfloat(argv);
    synth.setNoiseMix(mix);
}

// Sets the FM modulation index [fmmod f(
void jpvoice_tilde_modidx(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - n for FM/PM modulation index: [modidx f(");
        return;
    }

    host_float idx = atom_getfloat(argv);
    synth.setModulation(idx);
}

// Sets the pitch bend [bend f(
void jpvoice_tilde_bend(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - n for FM/PM modulation index: [modidx f(");
        return;
    }

    host_float bend = atom_getfloat(argv);
    synth.setPitchBend(bend);
}

// Sets the number of voices 1 - 9 [nov f(
void jpvoice_tilde_nov(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 9 for number of voices: [nov f(");
        return;
    }

    int nov = atom_getint(argv);
    synth.setNumVoices(nov);
}

// Oscillator sync
void jpvoice_tilde_sync(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 (unsynced) or 1 (synced) for oscillator sync: [oscsync n(");
        return;
    }

    int enabled = atom_getfloat(argv);
    synth.setSyncEnabled(enabled != 0);
}

// [filtermode <0|1|2>] → 0 = LPF12, 1 = BPF12, 2 = HPF12
void jpvoice_tilde_mode(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: mode <0=LP | 1=HP>");
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

// [carrierfb (0 - 1.2)]
void jpvoice_tilde_carrierfb(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: carrierfb (amount 0.0 – 1.2)");
        return;
    }

    host_float fb = atom_getfloat(argv);
    synth.setFeedbackCarrier(fb);
}

// [carrierfb (0 - 1.2)]
void jpvoice_tilde_modulatorfb(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: modulatorfb (amount 0.0 – 1.2)");
        return;
    }

    host_float fb = atom_getfloat(argv);
    synth.setFeedbackModulator(fb);
}

void jpvoice_tilde_cutoff(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: filter cutoff (amount 0.0 – samplerate / 2)");
        return;
    }

    host_float f = atom_getfloat(argv);
    synth.setFilterCutoff(f);
}

void jpvoice_tilde_reso(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: filter reso (amount 0.0 – 1.0)");
        return;
    }

    host_float r = atom_getfloat(argv);
    synth.setFilterResonance(r);
}

void jpvoice_tilde_drive(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: filter drive (amount 0.0 - 1.0)");
        return;
    }

    host_float d = atom_getfloat(argv);
    synth.setFilterDrive(d);
}

// Filter ADSR [fltadsr att dec sus rel cutoff attshape relshape(
void jpvoice_tilde_fltadsr(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpvoice~]: expected filter ADSR settings [fltadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    x->filterADSR.attackTime = atom_getfloat(&argv[0]);
    x->filterADSR.decayTime = atom_getfloat(&argv[1]);
    x->filterADSR.sustainLevel = atom_getfloat(&argv[2]);
    x->filterADSR.releaseTime = atom_getfloat(&argv[3]);
    x->filterADSR.attackShape = atom_getfloat(&argv[4]);
    x->filterADSR.releaseShape = atom_getfloat(&argv[5]);

    synth.setFilterADSR(x->filterADSR);
}

// Filter ADSR [ampadsr att dec sus rel cutoff attshape relshape(
void jpvoice_tilde_ampadsr(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpvoice~]: expected amp ADSR settings [ampadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    x->ampADSR.attackTime = atom_getfloat(&argv[0]);
    x->ampADSR.decayTime = atom_getfloat(&argv[1]);
    x->ampADSR.sustainLevel = atom_getfloat(&argv[2]);
    x->ampADSR.releaseTime = atom_getfloat(&argv[3]);
    x->ampADSR.attackShape = atom_getfloat(&argv[4]);
    x->ampADSR.releaseShape = atom_getfloat(&argv[5]);

    synth.setAmpADSR(x->ampADSR);
}

void jpvoice_tilde_adsrlink(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: adsrlink [adsrlink 0|1(");
        return;
    }

    synth.linkADSR(atom_getfloat(argv) != 0.0);
}

void jpvoice_tilde_adsroneshot(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: adsroneshot [adsroneshot 0|1(");
        return;
    }

    synth.setADSROneshot(atom_getfloat(argv) != 0.0);
}

// LFO1 [lfo1 type freq offset depth shape pw smooth target(
void jpvoice_tilde_lfo1(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpvoice~]: expected filter ADSR settings [fltadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    x->filterADSR.attackTime = atom_getfloat(&argv[0]);
    x->filterADSR.decayTime = atom_getfloat(&argv[1]);
    x->filterADSR.sustainLevel = atom_getfloat(&argv[2]);
    x->filterADSR.releaseTime = atom_getfloat(&argv[3]);
    x->filterADSR.attackShape = atom_getfloat(&argv[4]);
    x->filterADSR.releaseShape = atom_getfloat(&argv[5]);

    synth.setFilterADSR(x->filterADSR);
}

// LFO2 [lfo2 type freq offset depth shape pw smooth target(
void jpvoice_tilde_lfo2(t_jpvoice * x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 6)
    {
        pd_error(x, "[jpvoice~]: expected filter ADSR settings [fltadsr att dec sus rel cutoff attshape relshape(");
        return;
    }

    x->filterADSR.attackTime = atom_getfloat(&argv[0]);
    x->filterADSR.decayTime = atom_getfloat(&argv[1]);
    x->filterADSR.sustainLevel = atom_getfloat(&argv[2]);
    x->filterADSR.releaseTime = atom_getfloat(&argv[3]);
    x->filterADSR.attackShape = atom_getfloat(&argv[4]);
    x->filterADSR.releaseShape = atom_getfloat(&argv[5]);

    synth.setFilterADSR(x->filterADSR);
}

void jpvoice_tilde_revroom(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: revroom (amount 0.0 - 1.0)");
        return;
    }

    host_float room = atom_getfloat(argv);
    synth.setReverbRoom(room);
}

void jpvoice_tilde_revspace(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: revspace (amount 0.0 - 1.0)");
        return;
    }

    host_float space = atom_getfloat(argv);
    synth.setReverbSpace(space);
}

void jpvoice_tilde_revdamp(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: revdamp (amount 0.0 - 10000.0)");
        return;
    }

    host_float damping = atom_getfloat(argv);
    synth.setReverbDamping(damping);
}

void jpvoice_tilde_revdense(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: revdense (amount 0.0 - 1,0)");
        return;
    }

    host_float density = atom_getfloat(argv);
    synth.setReverbDensity(density);
}

void jpvoice_tilde_revdisp(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: revdisp (amount 0.0 - 1.0)");
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

void jpvoice_tilde_revwet(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: revvol (revvol 0.0 - 1,0)");
        return;
    }

    host_float v = atom_getfloat(argv);
    synth.setReverbWet(v);
}

void jpvoice_tilde_deltime(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: deltime (deltime tL tR)");
        return;
    }

    host_float tL = atom_getfloat(&argv[0]);
    host_float tR = atom_getfloat(&argv[1]);

    synth.setDelayTime(tL, tR);
}

void jpvoice_tilde_delfb(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: delfb (deltime fbL fbR)");
        return;
    }

    host_float fbL = atom_getfloat(&argv[0]);
    host_float fbR = atom_getfloat(&argv[1]);

    synth.setDelayFeedback(fbL, fbR);
}

void jpvoice_tilde_deldisp(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: deldisp (deldisp 0.0 - 1.0)");
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

void jpvoice_tilde_delwet(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: delwet (delwet 0.0 - 1,0)");
        return;
    }

    host_float v = atom_getfloat(argv);
    synth.setDelayWet(v);
}

void jpvoice_tilde_wet(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom * argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: wet (wet 0.0 - 1,0)");
        return;
    }

    host_float w = atom_getfloat(argv);
    synth.setWet(w);
}

// DSP perform function
t_int *jpvoice_tilde_perform(t_int *w)
{
    synth.processBlock();
    return (w + 5);
}

// DSP add function
void jpvoice_tilde_dsp(t_jpvoice *x, t_signal **sp)
{
    t_sample *outL = sp[0]->s_vec;
    t_sample *outR = sp[1]->s_vec;

    x->samplerate = sp[0]->s_sr;
    x->blockSize = sp[0]->s_n;

    DSP::initializeAudio(x->samplerate, x->blockSize);

    synth.initialize(outL, outR);

    dsp_add(jpvoice_tilde_perform, 4,
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
void *jpvoice_tilde_new()
{
    t_jpvoice *x = (t_jpvoice *)pd_new(jpvoice_class);

    x->left_out = outlet_new(&x->x_obj, &s_signal);
    x->right_out = outlet_new(&x->x_obj, &s_signal);

    DSP::registerLogger(&log);

    return (void *)x;
}

// Destructor
void jpvoice_tilde_free(t_jpvoice *x)
{
    outlet_free(x->left_out);
    outlet_free(x->right_out);

    // delete x->voice;
}

// Setup function
extern "C" void jpvoice_tilde_setup(void)
{
    jpvoice_class = class_new(gensym("jpvoice~"),
                              (t_newmethod)jpvoice_tilde_new,
                              (t_method)jpvoice_tilde_free,
                              sizeof(t_jpvoice),
                              CLASS_DEFAULT,
                              A_NULL);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_dsp, gensym("dsp"), A_CANT, 0);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_note, gensym("note"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_offset, gensym("offset"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fine, gensym("fine"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_detune, gensym("detune"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_carrier, gensym("carrier"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modulator, gensym("modulator"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_oscmix, gensym("oscmix"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_noisemix, gensym("noisemix"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_noisetype, gensym("noisetype"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modidx, gensym("modidx"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_bend, gensym("bend"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_nov, gensym("nov"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_sync, gensym("sync"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_carrierfb, gensym("carrierfb"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modulatorfb, gensym("modulatorfb"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_cutoff, gensym("cutoff"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_reso, gensym("reso"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_drive, gensym("drive"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_mode, gensym("mode"), A_GIMME, 0);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fltadsr, gensym("fltadsr"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_ampadsr, gensym("ampadsr"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_adsrlink, gensym("adsrlink"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_adsroneshot, gensym("adsroneshot"), A_GIMME, 0);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_lfo1, gensym("lfo1"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_lfo2, gensym("lfo2"), A_GIMME, 0);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_revroom, gensym("revroom"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_revspace, gensym("revspace"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_revdamp, gensym("revdamp"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_revdense, gensym("revdense"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_revdisp, gensym("revdisp"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_revwet, gensym("revwet"), A_GIMME, 0);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_deltime, gensym("deltime"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_delfb, gensym("delfb"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_deldisp, gensym("deldisp"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_delwet, gensym("delwet"), A_GIMME, 0);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_wet, gensym("wet"), A_GIMME, 0);
}
