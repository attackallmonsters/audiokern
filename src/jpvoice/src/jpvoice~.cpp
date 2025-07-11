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
void jpvoice_tilde_filtermode(t_jpvoice * /*x*/, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: filtermode <1=LPF12 | 2=BPF12 | 3=HPF12>");
        return;
    }

    int mode = atom_getint(argv);
    if (mode < 0 || mode > 2)
    {
        post("[jpvoice~] filtermode out of range 1 - 3, clamped.");
    }

    // TODO
    //synth.setFilterMode(static_cast<FilterMode>(clamp(mode, 1, 3)));
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

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fltadsr, gensym("fltadsr"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_ampadsr, gensym("ampadsr"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_adsrlink, gensym("adsrlink"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_adsroneshot, gensym("adsroneshot"), A_GIMME, 0);
}
