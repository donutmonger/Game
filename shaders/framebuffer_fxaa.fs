// Implementation of algorithm described here:
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf

#version 330

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D base_texture;

// Control the debug output
bool DEBUG = false;
bool FXAA_ON = true;

// Neighbor contrast tuning
float FXAA_EDGE_THRESHOLD_MIN = 1.0 / 16.0;
float FXAA_EDGE_THRESHOLD = 1.0 / 8.0;

// Subpixel contrast tuning
int FXAA_SUBPIX = 1;
float FXAA_SUBPIX_TRIM_SCALE = 1.0 / 2.0;
float FXAA_SUBPIX_CAP = 3.0 / 4.0;

// End of edge search tuning
int FXAA_SEARCH_STEPS = 4;
int FXAA_SEARCH_ACCELERATION = 2;
float FXAA_SEARCH_THRESHOLD = 1.0 / 4.0;

float FxaaLuma(vec4 color) {
    // As an optimization, luminance is estimated strictly from Red and Green
    // channels using a single fused multiply add operation. In practice pure
    // blue aliasing rarely appears in typical game content.
    return color.g * (0.587/0.299) + color.r;
}

void main() {
    //////////////////////////
    // Local Contrast Check
    vec4 current_pixel = texture(base_texture, Texcoord);
    float current_luminance = FxaaLuma(current_pixel);

    // Find the luminance of each neighboring pixel. (4x)
    vec4 n_pixel = textureOffset(base_texture, Texcoord, ivec2(0, 1));
    vec4 s_pixel = textureOffset(base_texture, Texcoord, ivec2(0, -1));
    vec4 e_pixel = textureOffset(base_texture, Texcoord, ivec2(1, 0));
    vec4 w_pixel = textureOffset(base_texture, Texcoord, ivec2(-1, 0));
    float n_luminance = FxaaLuma(n_pixel);
    float s_luminance = FxaaLuma(s_pixel);
    float e_luminance = FxaaLuma(e_pixel);
    float w_luminance = FxaaLuma(w_pixel);

    // Find the pixel with the smallest luminance
    float minimum_luminance = min(current_luminance, min(min(n_luminance, s_luminance),
    min(e_luminance, w_luminance)));

    // Find the pixel with the largest luminance
    float maximum_luminance = max(current_luminance, max(max(n_luminance, s_luminance),
    max(e_luminance, w_luminance)));

    // Find the range of luminance
    float range = maximum_luminance - minimum_luminance;

    // The threshold of the luminance range
    float threshold = max(FXAA_EDGE_THRESHOLD_MIN, maximum_luminance * FXAA_EDGE_THRESHOLD);

    bool is_contrast = range > threshold;

    //////////////////////////
    // Subpixel Aliasing Test
    float lumaL = (n_luminance + w_luminance + e_luminance +
        s_luminance) * 0.25;

    float rangeL = abs(lumaL - current_luminance);
    float blendL = max(0.0,
        (rangeL / range) - FXAA_SUBPIX_TRIM_SCALE) * FXAA_SUBPIX_TRIM_SCALE;
        blendL = min(FXAA_SUBPIX_CAP, blendL);

    vec4 pixelL = n_pixel + w_pixel + current_pixel + e_pixel
        + s_pixel;

    vec4 nw_pixel = textureOffset(base_texture, Texcoord, ivec2(-1,-1));
    vec4 ne_pixel = textureOffset(base_texture, Texcoord, ivec2( 1,-1));
    vec4 sw_pixel = textureOffset(base_texture, Texcoord, ivec2(-1, 1));
    vec4 se_pixel = textureOffset(base_texture, Texcoord, ivec2( 1, 1));
    float nw_luminance = FxaaLuma(nw_pixel);
    float ne_luminance = FxaaLuma(ne_pixel);
    float sw_luminance = FxaaLuma(sw_pixel);
    float se_luminance = FxaaLuma(se_pixel);

    pixelL += (nw_pixel + ne_pixel + sw_pixel + se_pixel);
    pixelL = pixelL/9.0;

    //////////////////////////
    // Vertical/Horizontal Edge Test
    float edgeVert =
        abs((0.25 * nw_luminance) + (-0.5 * n_luminance) + (0.25 * ne_luminance)) +
        abs((0.50 * w_luminance ) + (-1.0 * current_luminance) + (0.50 * e_luminance )) +
        abs((0.25 * sw_luminance) + (-0.5 * s_luminance) + (0.25 * se_luminance));
    float edgeHorz =
        abs((0.25 * nw_luminance) + (-0.5 * w_luminance) + (0.25 * sw_luminance)) +
        abs((0.50 * n_luminance ) + (-1.0 * current_luminance) + (0.50 * s_luminance )) +
        abs((0.25 * ne_luminance) + (-0.5 * e_luminance) + (0.25 * se_luminance));
    bool horzSpan = edgeHorz >= edgeVert;

    //////////////////////////
    // End of edge search


    if (is_contrast && FXAA_ON){
        if (DEBUG){
            if (horzSpan){
                outColor = vec4(1.0, 1.0, 0.0, 1.0);
            } else {
                outColor = vec4(0.0, 0.0, 1.0, 1.0);
            }
        } else {
            outColor = pixelL;
        }
    } else {
        outColor = current_pixel;
    }
}
