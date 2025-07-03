// lfo~.cpp – Efficient LFO external for Pure Data with smoothing for hard edges

#pragma GCC diagnostic ignored "-Wcast-function-type"

#include "m_pd.h"
#include "LFO.h"
#include "pdbase.h"
#include <cmath>
#include <cstdlib>

static t_class *lfo_tilde_class;

typedef struct t_lfo_tilde t_lfo_tilde;
typedef float (*lfo_func_ptr)(t_lfo_tilde *);

typedef struct t_lfo_tilde
{
    t_object x_obj;

    LFO *lfo;

    t_float samplerate;
    t_float blockSize;

    t_sample inletValue;

    t_sample *in_fm;
    t_outlet *x_out_sig;
    t_outlet *x_out_bang;
} t_lfo_tilde;

void lfo_tilde_reset(t_lfo_tilde *x)
{
    x->lfo->reset();
}

void lfo_tilde_setoffset(t_lfo_tilde *x, t_floatarg f)
{
    x->lfo->setOffset(f);
}

void lfo_tilde_setdepth(t_lfo_tilde *x, t_floatarg f)
{
    x->lfo->setDepth(f);
}

void lfo_tilde_setshape(t_lfo_tilde *x, t_floatarg f)
{
    x->lfo->setShape(f);
}

void lfo_tilde_setpw(t_lfo_tilde *x, t_floatarg f)
{
    x->lfo->setPulseWidth(f);
}

void lfo_tilde_setsmooth(t_lfo_tilde *x, t_floatarg f)
{
    x->lfo->setSmooth(f);
}

void lfo_tilde_setfreq(t_lfo_tilde *x, t_floatarg f)
{
    x->lfo->setFreq(f);
}

void lfo_tilde_settype(t_lfo_tilde *x, t_floatarg f)
{
    int type = static_cast<int>(f);

    switch (type)
    {
    case 0:
        x->lfo->setType(LFOType::Sine);
        break;
    case 1:
        x->lfo->setType(LFOType::RampUp);
        break;
    case 2:
        x->lfo->setType(LFOType::RampDown);
        break;
    case 3:
        x->lfo->setType(LFOType::Triangle);
        break;
    case 4:
        x->lfo->setType(LFOType::Square);
        break;
    case 5:
        x->lfo->setType(LFOType::Random);
        break;
    default:
        x->lfo->setType(LFOType::Sine);
        break;
    }
}

t_int *lfo_tilde_perform(t_int *w)
{
    t_lfo_tilde *x = (t_lfo_tilde *)(w[1]);
    t_sample *in_fm = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);

    x->lfo->generateBlock();

    dsp_float *buf = x->lfo->getBuffer();

    for (int i = 0; i < n; ++i)
    {
        out[i] = static_cast<t_sample>(buf[i]);
    }

    return (w + 5);
}

void lfo_tilde_dsp(t_lfo_tilde *x, t_signal **sp)
{
    x->samplerate = sp[0]->s_sr;
    x->blockSize = sp[0]->s_n;

    // dsp_add(lfo_tilde_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
    dsp_add(lfo_tilde_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);

    DSP::initializeAudio(x->samplerate, x->blockSize);

    x->lfo->initialize();
}

void *lfo_tilde_new(t_floatarg f)
{
    t_lfo_tilde *x = (t_lfo_tilde *)pd_new(lfo_tilde_class);

    DSP::registerLogger(&log);

    x->lfo = new LFO();
    x->lfo->setIdleSignal(f);
    x->lfo->onPhaseWrap = [x]()
    {
        outlet_bang(x->x_out_bang);
    };

    x->x_out_sig = outlet_new(&x->x_obj, &s_signal);
    x->x_out_bang = outlet_new(&x->x_obj, &s_bang);

    return (void *)x;
}

void lfo_tilde_free(t_lfo_tilde *x)
{
    delete x->lfo;
}

extern "C"
{
    void lfo_tilde_setup(void)
    {
        lfo_tilde_class = class_new(gensym("lfo~"),
                                    (t_newmethod)lfo_tilde_new,
                                    (t_method)lfo_tilde_free,
                                    sizeof(t_lfo_tilde),
                                    CLASS_DEFAULT, A_DEFFLOAT, 0);

        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_dsp, gensym("dsp"), A_CANT, 0);
        CLASS_MAINSIGNALIN(lfo_tilde_class, t_lfo_tilde, inletValue);

        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_setfreq, gensym("freq"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_settype, gensym("type"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_setoffset, gensym("offset"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_setdepth, gensym("depth"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_setshape, gensym("shape"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_setpw, gensym("pw"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_setsmooth, gensym("smooth"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_tilde_class, (t_method)lfo_tilde_reset, gensym("reset"), A_NULL);
    }
}
