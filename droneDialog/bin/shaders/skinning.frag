varying vec4 picking;
varying float zPos;

void main(){

    gl_FragData[0]=gl_Color;
    gl_FragData[1]=vec4(1.0,1.0,1.0,zPos);
    gl_FragData[2]=picking;
    gl_FragData[3]=vec4(0.0,0.0,0.0,1.0);

}


