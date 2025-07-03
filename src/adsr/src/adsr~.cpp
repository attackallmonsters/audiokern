// adsr~.cpp — Pure Data external for a nonlinear ADSR envelope with clamping and validation
#pragma GCC diagnostic ignored "-Wcast-function-type"

#include "m_pd.h"
#include "pdbase.h"
#include "ADSR.h"

static t_class *adsr_tilde_class;

// typedef struct enter_phase enter_phase;
typedef struct t_adsr_tilde t_adsr_tilde;

// === Main object structure ===
struct t_adsr_tilde
{
    t_object x_obj;
    t_outlet *x_out;

    dsp_float samplerate;
    size_t blockSize;
    double inletValue;

    ADSR *adsr;
};

// === Trigger methods ===
void adsr_trigger_start(t_adsr_tilde *x)
{
    x->adsr->triggerStart();
}

void adsr_trigger_stop(t_adsr_tilde *x)
{
    x->adsr->triggerStop();
}

// === Parameter setters with clamping ===
void adsr_attack(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setAttack(f);
}

void adsr_decay(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setDecay(f);
}

void adsr_release(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setRelease(f);
}

void adsr_sustain(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setSustain(f);
}

void adsr_attackshape(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setAttackShape(f);
}

void adsr_releaseshape(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setReleaseShape(f);
}

void adsr_g(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setGain(f);
}

void adsr_oneshot(t_adsr_tilde *x, t_floatarg f)
{
    x->adsr->setOneShot(f != 0.0);
}

// === Signal processing function ===
t_int *adsr_perform(t_int *w)
{
    t_adsr_tilde *x = (t_adsr_tilde *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);

    x->adsr->generateBlock();
    
    dsp_float* buf = x->adsr->getBuffer();

    for (int i = 0; i < n; ++i)
    {
        out[i] = static_cast<t_sample>(buf[i]);
    }

    return (w + 4);
}

void adsr_dsp(t_adsr_tilde *x, t_signal **sp)
{
    x->samplerate = sp[0]->s_sr;
    x->blockSize = sp[0]->s_n;
    
    DSP::initializeAudio(x->samplerate, x->blockSize);
    
    x->adsr->initialize();

    dsp_add(adsr_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

// === Object constructor ===
void *adsr_new(t_floatarg f)
{
    t_adsr_tilde *x = (t_adsr_tilde *)pd_new(adsr_tilde_class);
    x->x_out = outlet_new(&x->x_obj, &s_signal);

    DSP::registerLogger(&log);

    x->adsr = new ADSR();
    x->adsr->setStartAtCurrent(f != 0);

    return (void *)x;
}

void adsr_tilde_free(t_adsr_tilde *x)
{
    delete x->adsr;
}

// === Setup function ===
extern "C"
{
    void adsr_tilde_setup(void)
    {
        adsr_tilde_class = class_new(gensym("adsr~"),
                                     (t_newmethod)adsr_new,
                                     (t_method)adsr_tilde_free,
                                     sizeof(t_adsr_tilde),
                                     CLASS_DEFAULT,
                                     A_DEFFLOAT,
                                     0);

        class_addmethod(adsr_tilde_class, (t_method)adsr_dsp, gensym("dsp"), A_CANT, 0);
        CLASS_MAINSIGNALIN(adsr_tilde_class, t_adsr_tilde, inletValue);

        class_addmethod(adsr_tilde_class, (t_method)adsr_trigger_start, gensym("start"), A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_trigger_stop, gensym("stop"), A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_attack, gensym("attack"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_decay, gensym("decay"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_sustain, gensym("sustain"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_release, gensym("release"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_attackshape, gensym("attackshape"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_releaseshape, gensym("releaseshape"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_g, gensym("g"), A_DEFFLOAT, A_NULL);
        class_addmethod(adsr_tilde_class, (t_method)adsr_oneshot, gensym("oneshot"), A_DEFFLOAT, A_NULL);
    }
}
