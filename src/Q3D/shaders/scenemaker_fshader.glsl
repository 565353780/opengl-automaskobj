#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform vec4 u_color;
uniform mat4 model_matrix;
uniform int u_showtype;
uniform float u_pack_minval;
uniform float u_pack_scale;
uniform int u_selected;

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 v_view_normal;
varying vec4 v_frag_pos;
varying vec3 v_pos;

vec4 packInt(const in float val)
{
    vec4 v;

    v.x = floor(mod(float(val), 256.));
    v.y = floor(mod(float(val) / 256.0 , 256.0));
    v.z = floor(mod(float(val) / 256. / 256. , 256.));
    v.w = 255.0 - floor(mod(float(val) / 256. / 256. / 256., 256.));
    v /= 255.0;
    return v;
}
vec4 addPackInt(vec4 v0,  vec4 v1)
{
    v0 =floor(v0* 255.0);
    v1 =floor(v1 * 255.0);
    v0.w = 255.0 - v0.w;
    v1.w = 255.0 - v1.w;

    vec4 ans;
    ans.x = v0.x + v1.x;

    ans.y = floor(ans.x / 256.0) + v0.y + v1.y;
    ans.x = floor(mod(ans.x, 256.0));

    ans.z = floor(ans.y / 256.0) + v0.z + v1.z;
    ans.y = floor(mod(ans.y, 256.0));

    ans.w = floor(ans.z / 256.0) + v0.w + v1.w;
    ans.z = floor(mod(ans.z, 256.0));

    ans.w = 255.0 - ans.w;
    ans /= 255.0;
    return ans;
}
float unpackInt(const in vec4 v)
{
    float sum = 0.0;
    vec4 v1 = floor(v * 255.0);
    sum += (v1.x);
    sum += (v1.y) * 256.;
    sum += (v1.z) * 256. * 256.;
    sum += (255.0 - v1.w) * 256. * 256. * 256.;
    return sum;
}


//! [0]
void main()
{

    // Set fragment color from texture
    float lightness = abs((dot(normalize(v_view_normal),normalize(v_frag_pos.xyz)))) + 0.1;
    vec3 scolor = u_color.xyz;
    if(u_selected > 0)
    {
        scolor =  vec3(1.0,0.8,0.2);
    }
    gl_FragColor = vec4(scolor * lightness,1.0);

    if(u_showtype == 1)
    {
       gl_FragColor = packInt((v_pos.z - u_pack_minval)* u_pack_scale);
    }
    else if(u_showtype == 2)
    {
        gl_FragColor = packInt((v_pos.x - u_pack_minval)* u_pack_scale);
    }
    else if(u_showtype == 3)
    {
        gl_FragColor = packInt((v_pos.y - u_pack_minval)* u_pack_scale);
    }

//    gl_FragColor = vec4(vec3(v_view_normal + 1.0) * 0.5,1.0);
//    gl_FragColor = vec4(vec3(normalize(v_view_normal) + 1.0) * 0.5,1.0);

//    float precition = 30.0;
//    float zz = fract(v_frag_pos.z / precition);
//    vec3 color = vec3(1.,1.,0.) * zz + vec3(0.,1.,1.0) * (1.0 - zz);
//    gl_FragColor = vec4(color,1.0);

//    gl_FragColor= texture2D(texture, v_texcoord);
}
//! [0]

