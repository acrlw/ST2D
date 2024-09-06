#version 330 core

in vec4 PointColor;
in float PointRadius;

out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    if (dist > 0.5)
        discard;

    float smoothWidth = 1.0 / PointRadius;
    float alpha = 1.0 - smoothstep(0.5 - smoothWidth, 0.5, dist);

	FragColor = vec4(PointColor.rgb, alpha * PointColor.a);
}
