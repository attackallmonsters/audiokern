// jpvoice.cpp - Pure Data external wrapping the Voice audio synthesis class

#include "m_pd.h"
#include "pdbase.h"
#include "DSP.h"
#include "JPVoice.h"
#include "clamp.h"
#include "dsp_types.h"

static t_class *jpvoice_class;

typedef struct _jpvoice
{
    t_object x_obj;
    JPVoice *voice;

    t_inlet *in_cutoff;
    t_inlet *in_reso;
    t_outlet *left_out;
    t_outlet *right_out;
    dsp_float left;
    dsp_float right;
    dsp_float samplerate;
    size_t blockSize;

    DSPBuffer cutoffBuf;
    DSPBuffer resoBuf;
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
void jpvoice_tilde_f(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for carrier frequency f1: [f1 f(");
        return;
    }
    dsp_float f = atom_getfloat(argv);

    x->voice->setFrequency(f);
}

// Frequency offset modulator in halftones
void jpvoice_tilde_offset(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float offset = clamp(atom_getfloat(argv), -24.0f, 24.0f);
    x->voice->setPitchOffset(offset);
}

// Frequency fine tuning for modulator -100 - 100 [fine f(
void jpvoice_tilde_fine(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float finetune = clamp(atom_getfloat(argv), -100.0f, 100.0f);
    x->voice->setFineTune(finetune);
}

// Frequency of modulator set via list [detune factor(
void jpvoice_tilde_detune(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float detune = clamp(atom_getfloat(argv), 0.0f, 1.0f);
    x->voice->setDetune(detune);
}

// Oscillator type carrier [carrier n( 1 - 5
void jpvoice_tilde_carrier(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Saw);
        break;
    case 2:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Square);
        break;
    case 3:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Triangle);
        break;
    case 4:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Sine);
        break;
    case 5:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Cluster);
        break;
    case 6:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Fibonacci);
        break;
    case 7:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Mirror);
        break;
    case 8:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Modulo);
        break;
    default:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Saw);
        break;
    }
}

// Oscillator type carrier [modulator n( 1 - 4
void jpvoice_tilde_modulator(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Saw);
        break;
    case 2:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Square);
        break;
    case 3:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Triangle);
        break;
    case 4:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Sine);
        break;
    case 5:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Cluster);
        break;
    case 6:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Fibonacci);
        break;
    case 7:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Mirror);
        break;
    case 8:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Modulo);
        break;
    case 9:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Bit);
        break;
    default:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Sine);
        break;
    }
}

// Sets the type of noise to white (0) or pink (1)
void jpvoice_tilde_noisetype(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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
        x->voice->setNoiseType(NoiseType::White);
        break;
    case 1:
        x->voice->setNoiseType(NoiseType::Pink);
        break;
    default:
        x->voice->setNoiseType(NoiseType::White);
        break;
    }
}

// Oscillator mix [oscmix f(
void jpvoice_tilde_oscmix(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    float mix = clamp(static_cast<float>(atom_getfloat(argv)), 0.0f, 1.0f);
    x->voice->setOscillatorMix(mix);
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

    float mix = clamp(static_cast<float>(atom_getfloat(argv)), 0.0f, 1.0f);
    x->voice->setNoiseMix(mix);
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

    dsp_float idx = clampmin(static_cast<float>(atom_getfloat(argv)), 0.0f);
    x->voice->setModIndex(idx);
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

    int nov = clamp(static_cast<int>(atom_getfloat(argv)), 0.0f, 9.0f);
    x->voice->setNumVoices(nov);
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

    int enabled = clamp(static_cast<int>(atom_getint(argv)), 0, 1);
    x->voice->setSyncEnabled(enabled == 1);
}

// [filtermode <0|1|2>] → 0 = LPF12, 1 = BPF12, 2 = HPF12
void jpvoice_tilde_filtermode(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    x->voice->setFilterMode(static_cast<FilterMode>(clamp(mode, 1, 3)));
}

// [carrierfb (0 - 1.2)]
void jpvoice_tilde_carrierfb(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float fb = atom_getfloat(argv);

    x->voice->setFeedbackCarrier(fb);
}

// [carrierfb (0 - 1.2)]
void jpvoice_tilde_modulatorfb(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float fb = atom_getfloat(argv);

    x->voice->setFeedbackModulator(fb);
}

void jpvoice_tilde_cutoff(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float cf = atom_getfloat(argv);

    x->cutoffBuf.fill(cf);
    x->voice->setFilterCutoff(&x->cutoffBuf);
}

void jpvoice_tilde_reso(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
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

    dsp_float r = atom_getfloat(argv);

    x->resoBuf.fill(r);
    x->voice->setFilterResonance(&x->resoBuf);
}

void jpvoice_tilde_drive(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (!testDSP())
    {
        return;
    }

    if (argc < 1)
    {
        post("[jpvoice~] usage: filter drive (amount 0.0 – 1.0)");
        return;
    }

    dsp_float d = atom_getfloat(argv);

    x->voice->setFilterDrive(d * 20.0);
}

// DSP perform function
t_int *jpvoice_tilde_perform(t_int *w)
{
    t_jpvoice *x = (t_jpvoice *)(w[1]);
    t_sample *cutoff = (t_sample *)(w[2]);
    t_sample *reso = (t_sample *)(w[3]);
    t_sample *outL = (t_sample *)(w[4]);
    t_sample *outR = (t_sample *)(w[5]);
    int n = (int)(w[6]);

    x->cutoffBuf.set(cutoff);
    x->voice->setFilterCutoff(&x->cutoffBuf);

    x->resoBuf.set(reso);
    x->voice->setFilterResonance(&x->resoBuf);

    x->voice->computeSamples();

    dsp_float *bufL = x->voice->mixBufferL.data();
    dsp_float *bufR = x->voice->mixBufferR.data();

    for (int i = 0; i < n; ++i)
    {
        outL[i] = static_cast<t_sample>(bufL[i]);
        outR[i] = static_cast<t_sample>(bufR[i]);
    }

    return (w + 7);
}

// DSP add function
void jpvoice_tilde_dsp(t_jpvoice *x, t_signal **sp)
{
    x->samplerate = sp[0]->s_sr;
    x->blockSize = sp[0]->s_n;
    
    DSP::initializeAudio(x->samplerate, x->blockSize);

    x->cutoffBuf.resize(x->blockSize);
    x->resoBuf.resize(x->blockSize);

    x->voice->initialize();

    dsp_add(jpvoice_tilde_perform, 6,
            x,
            sp[0]->s_vec, // in_cutoff
            sp[1]->s_vec, // in_reso
            sp[2]->s_vec, // outL
            sp[3]->s_vec, // outR
            sp[0]->s_n);
}

// Constructor
void *jpvoice_tilde_new()
{
    t_jpvoice *x = (t_jpvoice *)pd_new(jpvoice_class);

    // register logger for DSP objects
    DSP::registerLogger(&log);

    x->voice = new JPVoice();

    x->in_cutoff = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    x->in_reso = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

    x->left_out = outlet_new(&x->x_obj, &s_signal);
    x->right_out = outlet_new(&x->x_obj, &s_signal);

    return (void *)x;
}

// Destructor
void jpvoice_tilde_free(t_jpvoice *x)
{
    inlet_free(x->in_cutoff);
    inlet_free(x->in_reso);
    outlet_free(x->left_out);
    outlet_free(x->right_out);

    delete x->voice;
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
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_f, gensym("f"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_offset, gensym("offset"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fine, gensym("fine"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_detune, gensym("detune"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_carrier, gensym("carrier"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modulator, gensym("modulator"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_oscmix, gensym("oscmix"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_noisemix, gensym("noisemix"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_noisetype, gensym("noisetype"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modidx, gensym("modidx"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_nov, gensym("nov"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_sync, gensym("sync"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_carrierfb, gensym("carrierfb"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modulatorfb, gensym("modulatorfb"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_cutoff, gensym("cutoff"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_reso, gensym("reso"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_drive, gensym("drive"), A_GIMME, 0);
}
