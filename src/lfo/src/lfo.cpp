// lfo~.cpp – Efficient LFO external for Pure Data with smoothing for hard edges

#pragma GCC diagnostic ignored "-Wcast-function-type"

#include "m_pd.h"
#include "dsp_math.h"
#include "LFO.h"
#include <cmath>
#include <cstdlib>

static t_class *lfo_class;

typedef struct t_lfo t_lfo;
typedef float (*lfo_func_ptr)(t_lfo *);

typedef struct t_lfo
{
    t_object x_obj;

    LFO *lfo;

    t_float samplerate;
    t_float blockSize;

    t_sample inletValue;

    t_sample *in_fm;
    t_outlet *x_out_value;
    t_outlet *x_out_bang;
} t_lfo;

void lfo_reset(t_lfo *x)
{
    x-lfo.meset();
}

void lfo_setoffset(t_lfo *x, t_floatarg f)
{
    x-lfo->setOffset(f);
}

void lfo_setdepth(t_lfo *x, t_floatarg f)
{
    x-lfo->setDepth(f);
}

void lfo_setshape(t_lfo *x, t_floatarg f)
{
    x-lfo->setShape(f);
}

void lfo_setpw(t_lfo *x, t_floatarg f)
{
    x-lfo->setPulseWidth(f);
}

void lfo_setsmooth(t_lfo *x, t_floatarg f)
{
    x-lfo->setSmooth(f);
}

void lfo_unipolar(t_lfo *x, t_floatarg f)
{
    x-lfo->isUnipolar(f != 0.0);
}

void lfo_setfreq(t_lfo *x, t_floatarg f)
{
    x-lfo->setFrequency(f);
}

void lfo_settype(t_lfo *x, t_floatarg f)
{
    int type = static_cast<int>(f);

    switch (type)
    {
    case 0:
        x-lfo->setType(LFOType::Sine);
        break;
    case 1:
        x-lfo->setType(LFOType::RampUp);
        break;
    case 2:
        x-lfo->setType(LFOType::RampDown);
        break;
    case 3:
        x-lfo->setType(LFOType::Triangle);
        break;
    case 4:
        x-lfo->setType(LFOType::Square);
        break;
    case 5:
        x-lfo->setType(LFOType::Random);
        break;
    default:
        x-lfo->setType(LFOType::Sine);
        break;
    }
}

t_int *lfo_perform(t_int *w)
{
    t_lfo *x = (t_lfo *)(w[1]);

    x-lfo->process();

    return (w + 2);
}

void lfo_dsp(t_lfo *x, t_signal **sp)
{
    x->samplerate = sp[0]->s_sr;
    x->blockSize = sp[0]->s_n;

    dsp_add(lfo_perform, 1, x);

    DSP::initializeAudio(x->samplerate, x->blockSize);

    x-lfo->initialize(dsp_math::unique_string_id("value_lfo"));
    x-lfo->setMode(LFOMode::Value);
}

inline void log(const std::string &entry)
{
    post("%s", entry.c_str());
}

void *lfo_new(t_floatarg f)
{
    t_lfo *x = (t_lfo *)pd_new(lfo_class);

    DSP::registerLogger(&log);

    x-lfo = new LFO();
    x-lfo->setIdleSignal(f);
    
    x-lfo->onPhaseWrap = [x]()
    {
        outlet_bang(x->x_out_bang);
    };

    x-lfo->processLFOValue = [x](host_float v)
    {
        outlet_float(x->x_out_value, v);
    };

    x->x_out_value = outlet_new(&x->x_obj, &s_float);
    x->x_out_bang = outlet_new(&x->x_obj, &s_bang);

    return (void *)x;
}

void lfo_free(t_lfo *x)
{
    delete x-lfo;
}

extern "C"
{
    void lfo_setup(void)
    {
        lfo_class = class_new(gensym("lfo"),
                                    (t_newmethod)lfo_new,
                                    (t_method)lfo_free,
                                    sizeof(t_lfo),
                                    CLASS_DEFAULT, A_DEFFLOAT, 0);

        class_addmethod(lfo_class, (t_method)lfo_dsp, gensym("dsp"), A_CANT, 0);
        CLASS_MAINSIGNALIN(lfo_class, t_lfo, inletValue);

        class_addmethod(lfo_class, (t_method)lfo_setfreq, gensym("freq"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_settype, gensym("type"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_setoffset, gensym("offset"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_setdepth, gensym("depth"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_setshape, gensym("shape"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_setpw, gensym("pw"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_setsmooth, gensym("smooth"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_unipolar, gensym("unipolar"), A_DEFFLOAT, A_NULL);
        class_addmethod(lfo_class, (t_method)lfo_reset, gensym("reset"), A_NULL);
    }
}
