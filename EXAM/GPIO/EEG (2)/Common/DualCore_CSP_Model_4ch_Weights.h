#ifndef __DUALCORE_CSP_MODEL_4CH_WEIGHTS_H
#define __DUALCORE_CSP_MODEL_4CH_WEIGHTS_H

/* Placeholder CSP model.
 * PyCharm script python_csp/train_csp_lr.py will overwrite this file.
 * Channels are CP3, CP4, C3, C4. Labels are LEFT=2, RIGHT=3 in firmware logs.
 */
#define CSP4CH_INPUT_CH          4
#define CSP4CH_FEATURE_DIM       4
#define CSP4CH_WEIGHTS_TRAINED   0

static const float csp4ch_matrix[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

static const float csp4ch_mean[4] = {0.0f, 0.0f, 0.0f, 0.0f};
static const float csp4ch_scale[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const float csp4ch_svm_weight[4] = {0.0f, 0.0f, 0.0f, 0.0f};
static const float csp4ch_svm_bias = 0.0f;

#endif /* __DUALCORE_CSP_MODEL_4CH_WEIGHTS_H */
