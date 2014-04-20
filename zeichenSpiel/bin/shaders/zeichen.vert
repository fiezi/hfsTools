uniform float time;
uniform sampler2D tex;
uniform sampler2D tex2;


/*
*   Main
*/

void main(){

    //texturing
     gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
     gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord0;
    gl_TexCoord[1].s*=640;
    gl_TexCoord[1].t*=480;
    gl_FrontColor=gl_Color;
    vec4 myVertex=gl_Vertex;
    myVertex.w=1.0;


    //myVertex.z=texture2DRect(tex,gl_TexCoord[0].st).r;
    //myVertex.z=1.0;

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * myVertex;

}
