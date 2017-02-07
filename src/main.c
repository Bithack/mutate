
#include <stdlib.h>
#include <wand/magick_wand.h>
#include <argp.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

#define MT_NAME "mutate"
#define MT_VERSION "0.1"

#define LOG_I(x, ...) fprintf(stdout, x "\n", ##__VA_ARGS__)
#define LOG_II(x, ...) fprintf(stdout, x , ##__VA_ARGS__)
#define LOG_E(x, ...) fprintf(stderr, x "\n", ##__VA_ARGS__)

#define RNDFLT(min,max) (min + ((float)rand() /(float)(RAND_MAX))*(max-min))

const char *argp_program_version = MT_NAME " " MT_VERSION;
const char *argp_program_bug_address = "<mutate@teorem.se>";
static char doc[] = "mutate -- image mutation";
static char args_doc[] = "INPUT-FILES...";

static struct argp_option options[] = {
    {"chaos", 'c', "COEFF", 0, "Chaos level, used by some operations"},
    {"number", 'n', "NUMBER", 0, "Number of mutated output files to create"},
    {"operations", 'p', "NUMBER", 0, "Number of operations per mutated output file"},
    {"output", 'o', "DIR", 0, "Directory where to save outputs"},
    {0}
};

struct arguments {
    char **input_files;
    int num_input_files;

    char *output;

    int depth;
    int count;
    float chaos;
} arguments;

static error_t
parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'c':
            arguments->chaos = atof(arg);
            break;

        case 'n':
            arguments->count = atoi(arg);
            break;

        case 'o':
            arguments->output = arg;
            break;

        case 'p':
            arguments->depth = atoi(arg);
            break;

        case ARGP_KEY_ARGS:
            arguments->input_files = (char**)(state->argv + state->next);
            arguments->num_input_files = state->argc - state->next;
            break;

        case ARGP_KEY_END:
            if (arguments->num_input_files == 0) {
                argp_usage(state);
            }
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

void init_defaults(void);

struct op {
    const char *name;
    int (*fun)(MagickWand *);
};

int op_test(MagickWand *wand);
int op_contrast_stretch(MagickWand *wand);
int op_blur(MagickWand *wand);
int op_sharpen(MagickWand *wand);
int op_normalize(MagickWand *wand);
int op_hue(MagickWand *wand);
int op_saturation(MagickWand *wand);
int op_brightness(MagickWand *wand);
int op_sigmoidal_contrast_increase(MagickWand *wand);
int op_sigmoidal_contrast_decrease(MagickWand *wand);
int op_sigmoidal_contrast_decrease(MagickWand *wand);
int op_equalize(MagickWand *wand);
int op_enhance(MagickWand *wand);
int op_perspective(MagickWand *wand);
int op_rotate(MagickWand *wand);

/* special */
int op_charcoal(MagickWand *wand);
int op_solarize(MagickWand *wand);
int op_sketch(MagickWand *wand);
int op_swirl(MagickWand *wand);
int op_wave(MagickWand *wand);
int op_implode(MagickWand *wand);

int op_auto_level(MagickWand *wand);
int op_auto_gamma(MagickWand *wand);
int op_blueshift(MagickWand *wand);
int op_despeckle(MagickWand *wand);

struct op ops[] = {
    {"test", op_test},
    {"contrast_stretch", op_contrast_stretch},
    {"blur", op_blur},
    {"sharpen", op_sharpen},
    {"solarize", op_solarize},
    {"normalize", op_normalize},
    {"hue", op_hue},
    {"saturation", op_saturation},
    {"brightness", op_brightness},
    {"sigmoidal_contrast_increase", op_sigmoidal_contrast_increase},
    {"sigmoidal_contrast_decrease", op_sigmoidal_contrast_decrease},
    {"sketch", op_sketch},
    {"swirl", op_swirl},
    {"wave", op_wave},
    {"implode", op_implode},
    {"equalize", op_equalize},
    {"enhance", op_enhance},
    {"auto_level", op_auto_level},
    {"auto_gamma", op_auto_gamma},
    {"blueshift", op_blueshift},
    {"despeckle", op_despeckle},
    {"perspective", op_perspective},
    {"rotate", op_rotate},
    //{"charcoal", op_charcoal},
};

void sanity_check(void);

int main(int argc, char *argv[]) {
    char out_name[1024];
    init_defaults();
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    sanity_check();

    srand((int)time(0));

    MagickWand *wand;

    MagickWandGenesis();

    wand = NewMagickWand();

    for (int i=0; i<arguments.num_input_files; i++) {
        char *in = arguments.input_files[i], *s;
        if (!MagickReadImage(wand, in)) {
            LOG_E("could not open file: %s", in);
            continue;
        }

        if ((s = strchr(in, '/'))) {
            in = s;
        }
        if ((s = strchr(in, '.'))) {
            *s = '\0';
        }

        for (int c=0; c<arguments.count; c++) {
            MagickWand *work = CloneMagickWand(wand);

            LOG_II("%s-%d: ", in, c);

            for (int d=0; d<arguments.depth; d++) {
                struct op *op = &ops[rand() % (sizeof(ops)/sizeof(struct op))];
                LOG_II("%s", op->name);
                fflush(stdout);

                int ret = op->fun(work);
                if (ret != 1) {
                    LOG_E("%s: error: %d", op->name, ret);
                    break;
                }

                if (d != arguments.depth-1) {
                    LOG_II(", ");
                }
            }
            LOG_I("");

            /* crop the image back to its original size if changes */
            int hd = MagickGetImageHeight(wand) - MagickGetImageHeight(work);
            int wd = MagickGetImageWidth(wand) - MagickGetImageWidth(work);

            if (hd < 0 && wd <= 0)  {
                MagickCropImage(work,
                        MagickGetImageWidth(work) + wd,
                        MagickGetImageHeight(work) + hd,
                        wd / 2,
                        hd / 2
                    );
            }

            snprintf(out_name, 1024, "%s%s-%d.jpg", arguments.output, in, c);
            MagickWriteImage(work, out_name);

            DestroyMagickWand(work);
        }
    }

    DestroyMagickWand(wand);
    MagickWandTerminus();

    return 0;
}

void sanity_check(void) {
    int output_len = strlen(arguments.output);
    if (arguments.output[output_len-1] != '/') {
        char *ns = malloc(output_len+2); /* extra char + nullbyte */
        strcpy(ns, arguments.output);
        strcat(ns, "/");
        arguments.output = ns; /* yes, this leaks. */
    }

    DIR *dir = opendir(arguments.output);
    if (!dir) {
        LOG_E("error: %s: could not open directory", arguments.output);
        exit(1);
    }
}

void init_defaults(void) {
    arguments.count = 1;
    arguments.depth = 10;
    arguments.num_input_files = 0;
    arguments.chaos = 0.25;
    arguments.output = "./";
}

int op_contrast_stretch(MagickWand *wand) {
    float a = RNDFLT(0.0, 0.8);
    float b = RNDFLT(0.1, (1.0-a));
    return MagickContrastStretchImage(wand, a, a+b) == MagickTrue;
}

int op_blur(MagickWand *wand) {
    float a = RNDFLT(0.0, 10.0);
    return MagickBlurImage(wand, 0, a) == MagickTrue;
}

int op_sharpen(MagickWand *wand) {
    float a = RNDFLT(0.0, 5.0);
    return MagickSharpenImage(wand, 0, a) == MagickTrue;
}

int op_charcoal(MagickWand *wand) {
    return MagickCharcoalImage(wand, 0, 0.5) == MagickTrue;
}

int op_solarize(MagickWand *wand) {
    float a = RNDFLT(0.0, QuantumRange);
    return MagickSolarizeImage(wand, a) == MagickTrue;
}

int op_normalize(MagickWand *wand) {
    return MagickNormalizeImage(wand) == MagickTrue;
}

int op_hue(MagickWand *wand) {
    float a = RNDFLT(0.0, 100.0 * arguments.chaos) - 50.0 * arguments.chaos;
    return MagickModulateImage(wand, 100, 100, 100 + a) == MagickTrue;
}

int op_saturation(MagickWand *wand) {
    float a = RNDFLT(0.0, 100.0 * arguments.chaos) - 50.0 * arguments.chaos;
    return MagickModulateImage(wand, 100, 100 + a, 100) == MagickTrue;
}

int op_brightness(MagickWand *wand) {
    float a = RNDFLT(0.0, 100.0 * arguments.chaos) - 50.0 * arguments.chaos;
    return MagickModulateImage(wand, 100+a, 100, 100) == MagickTrue;
}

int op_sigmoidal_contrast_increase(MagickWand *wand) {
    float contrast = 2.5 + RNDFLT(0, 10.0 * arguments.chaos);
    float midpoint = 50.0;
    return MagickSigmoidalContrastImage(wand, MagickTrue, contrast, midpoint) == MagickTrue;
}
int op_sigmoidal_contrast_decrease(MagickWand *wand) {
    float contrast = 2.5 + RNDFLT(0, 10.0 * arguments.chaos);
    float midpoint = 50.0;
    return MagickSigmoidalContrastImage(wand, MagickFalse, contrast, midpoint) == MagickTrue;
}
int op_sketch(MagickWand *wand) {
    float sigma = RNDFLT(0.5, 20.0) * arguments.chaos;
    return MagickSketchImage(wand, 0.0, sigma, RNDFLT(0.0, 180.0)) == MagickTrue;
}

#define SWIRL_STRENGTH 12.0
#define WAVE_MAX_WAVE_LENGTH 100.0
#define WAVE_MAX_AMPLITUDE 0.5
#define IMPLODE_MAX 0.1
#define PERSPECTIVE_FACTOR 0.25
#define ROTATE_DEGREES_MAX 20.0

int op_swirl(MagickWand *wand) {
    float degrees = RNDFLT(0.0, SWIRL_STRENGTH * arguments.chaos) - (SWIRL_STRENGTH/2.0) * arguments.chaos;
    return MagickSwirlImage(wand, degrees) == MagickTrue;
}

int op_wave(MagickWand *wand) {
    float amplitude = WAVE_MAX_AMPLITUDE * arguments.chaos;
    float wave_length = RNDFLT(0, WAVE_MAX_WAVE_LENGTH);
    return MagickWaveImage(wand, amplitude, wave_length) == MagickTrue;
}

int op_implode(MagickWand *wand) {
    float amount = RNDFLT(0, IMPLODE_MAX * arguments.chaos);
    return MagickImplodeImage(wand, amount) == MagickTrue;
}

int op_equalize(MagickWand *wand) {
    return MagickEqualizeImage(wand) == MagickTrue;
}

int op_enhance(MagickWand *wand) {
    return MagickEnhanceImage(wand) == MagickTrue;
}

int op_auto_level(MagickWand *wand) {
    return MagickAutoLevelImage(wand) == MagickTrue;
}
int op_auto_gamma(MagickWand *wand) {
    return MagickAutoGammaImage(wand) == MagickTrue;
}
int op_blueshift(MagickWand *wand) {
    return MagickBlueShiftImage(wand, 1.0 + RNDFLT(0, 0.5)*arguments.chaos) == MagickTrue;
}
int op_despeckle(MagickWand *wand) {
    return MagickDespeckleImage(wand) == MagickTrue;
}

int op_perspective(MagickWand *wand) {
    float w = (float)MagickGetImageWidth(wand);
    float h = (float)MagickGetImageHeight(wand);

    float ws = (w * PERSPECTIVE_FACTOR);
    float hs = (h * PERSPECTIVE_FACTOR);

    LOG_II(" %f %f", ws, hs);

    double args[] = {
        0, 0, /* -> */ 0+ws*RNDFLT(-arguments.chaos, arguments.chaos), hs*RNDFLT(-arguments.chaos, arguments.chaos),
        w, 0, /* -> */ w+ws*RNDFLT(-arguments.chaos, arguments.chaos), hs*RNDFLT(-arguments.chaos, arguments.chaos),
        0, h, /* -> */ 0+ws*RNDFLT(-arguments.chaos, arguments.chaos), h+hs*RNDFLT(-arguments.chaos, arguments.chaos),
        w, h, /* -> */ w+ws*RNDFLT(-arguments.chaos, arguments.chaos), h+hs*RNDFLT(-arguments.chaos, arguments.chaos),
    };

    return MagickDistortImage(wand, PerspectiveDistortion, sizeof(args)/sizeof(double), args, MagickFalse) == MagickTrue;
}

int op_rotate(MagickWand *wand) {
    PixelWand *pw = NewPixelWand();
    PixelSetColor(pw, "#ffffffff");
    float degrees =  RNDFLT(-arguments.chaos*ROTATE_DEGREES_MAX, arguments.chaos*ROTATE_DEGREES_MAX);
    int ret = MagickRotateImage(wand, pw, degrees) == MagickTrue;
    LOG_II(" %.02f", degrees);
    DestroyPixelWand(pw);

    return ret;
}


int op_test(MagickWand *wand) {
    return 1;
}

