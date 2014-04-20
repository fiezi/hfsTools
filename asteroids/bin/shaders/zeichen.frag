uniform sampler2D tex;
uniform sampler2DRect tex2;
uniform float time;

vec2 tc_offset[25];

vec4 mask;
vec4 maskColor;
vec4 kinectColor;
vec4 color;

vec4 blur5(sampler2DRect myTex,vec2 tc,float mySpread){

      float spread=mySpread;// * min(0.0,max(4.0,unpackToFloat(texture2D(depthTex,texCoord).xy*farClip/100.0)));

      tc_offset[0]=spread * vec2(-2.0,-2.0);
      tc_offset[1]=spread * vec2(-1.0,-2.0);
      tc_offset[2]=spread * vec2(0.0,-2.0);
      tc_offset[3]=spread * vec2(1.0,-2.0);
      tc_offset[4]=spread * vec2(2.0,-2.0);

      tc_offset[5]=spread * vec2(-2.0,-1.0);
      tc_offset[6]=spread * vec2(-1.0,-1.0);
      tc_offset[7]=spread * vec2(0.0,-1.0);
      tc_offset[8]=spread * vec2(1.0,-1.0);
      tc_offset[9]=spread * vec2(2.0,-1.0);

      tc_offset[10]=spread * vec2(-2.0,0.0);
      tc_offset[11]=spread * vec2(-1.0,0.0);
      tc_offset[12]=spread * vec2(0.0,0.0);
      tc_offset[13]=spread * vec2(1.0,0.0);
      tc_offset[14]=spread * vec2(2.0,0.0);

      tc_offset[15]=spread * vec2(-2.0,1.0);
      tc_offset[16]=spread * vec2(-1.0,1.0);
      tc_offset[17]=spread * vec2(0.0,1.0);
      tc_offset[18]=spread * vec2(1.0,1.0);
      tc_offset[19]=spread * vec2(2.0,1.0);

      tc_offset[20]=spread * vec2(-2.0,2.0);
      tc_offset[21]=spread * vec2(-1.0,2.0);
      tc_offset[22]=spread * vec2(0.0,2.0);
      tc_offset[23]=spread * vec2(1.0,2.0);
      tc_offset[24]=spread * vec2(2.0,2.0);


      vec4 sample[25];

      for (int i=0 ; i<25 ; i++)
      {
        sample[i]=texture2DRect(myTex , tc + tc_offset[i]);
      }

      vec4 blurredColor=(
                        (1.0 * (sample[0] + sample[4] + sample[20] + sample[24])) +

                        (4.0 * (sample[1] + sample[3] + sample[5] + sample[9] +
                               sample[15] + sample[19] + sample[21] + sample[23])) +

                        (7.0 * (sample[2] + sample[10] + sample[14] + sample[22])) +

                        (16.0 * (sample[6] + sample[8] + sample[16] + sample[18])) +

                        (26.0 * (sample[7] + sample[11] + sample[13] + sample[17])) +
                        (41.0 * sample[12])
                        )/ 273.0;
      //blurredColor.a=1.0;
      return(blurredColor);
}

vec4 blur5tx(sampler2D myTex,vec2 tc,float mySpread){

      float spread=mySpread;// * min(0.0,max(4.0,unpackToFloat(texture2D(depthTex,texCoord).xy*farClip/100.0)));

      tc_offset[0]=spread * vec2(-2.0,-2.0);
      tc_offset[1]=spread * vec2(-1.0,-2.0);
      tc_offset[2]=spread * vec2(0.0,-2.0);
      tc_offset[3]=spread * vec2(1.0,-2.0);
      tc_offset[4]=spread * vec2(2.0,-2.0);

      tc_offset[5]=spread * vec2(-2.0,-1.0);
      tc_offset[6]=spread * vec2(-1.0,-1.0);
      tc_offset[7]=spread * vec2(0.0,-1.0);
      tc_offset[8]=spread * vec2(1.0,-1.0);
      tc_offset[9]=spread * vec2(2.0,-1.0);

      tc_offset[10]=spread * vec2(-2.0,0.0);
      tc_offset[11]=spread * vec2(-1.0,0.0);
      tc_offset[12]=spread * vec2(0.0,0.0);
      tc_offset[13]=spread * vec2(1.0,0.0);
      tc_offset[14]=spread * vec2(2.0,0.0);

      tc_offset[15]=spread * vec2(-2.0,1.0);
      tc_offset[16]=spread * vec2(-1.0,1.0);
      tc_offset[17]=spread * vec2(0.0,1.0);
      tc_offset[18]=spread * vec2(1.0,1.0);
      tc_offset[19]=spread * vec2(2.0,1.0);

      tc_offset[20]=spread * vec2(-2.0,2.0);
      tc_offset[21]=spread * vec2(-1.0,2.0);
      tc_offset[22]=spread * vec2(0.0,2.0);
      tc_offset[23]=spread * vec2(1.0,2.0);
      tc_offset[24]=spread * vec2(2.0,2.0);


      vec4 sample[25];

      for (int i=0 ; i<25 ; i++)
      {
        sample[i]=texture2D(myTex , tc + tc_offset[i]);
      }

      vec4 blurredColor=(
                        (1.0 * (sample[0] + sample[4] + sample[20] + sample[24])) +

                        (4.0 * (sample[1] + sample[3] + sample[5] + sample[9] +
                               sample[15] + sample[19] + sample[21] + sample[23])) +

                        (7.0 * (sample[2] + sample[10] + sample[14] + sample[22])) +

                        (16.0 * (sample[6] + sample[8] + sample[16] + sample[18])) +

                        (26.0 * (sample[7] + sample[11] + sample[13] + sample[17])) +
                        (41.0 * sample[12])
                        )/ 273.0;
      //blurredColor.a=1.0;
      return(blurredColor);
}


void offsetize(int offset){

    for (int i=0;i<offset;i++){
        float o=float(offset);
        float x=1.0;
        float y=1.0;
        mask = texture2D(tex,gl_TexCoord[0].st+vec2(o*-x/100.0,o*-y/100.0)+vec2(float(i)*x/100.0,float(i)*y/100.0));
        if (mask.r>0.8 && mask.r -color.r<0.5){
            maskColor.rgb=  float(i)/o;
        }
        x=1.0;
        y=0.0;
        mask = texture2D(tex,gl_TexCoord[0].st+vec2(o*-x/100.0,o*-y/100.0)+vec2(float(i)*x/100.0,float(i)*y/100.0));
        if (mask.r>0.8 && mask.r -color.r<0.5){
            maskColor.rgb=  float(i)/o;
        }
        x=-1.0;
        y=0.0;
        mask = texture2D(tex,gl_TexCoord[0].st+vec2(o*-x/100.0,o*-y/100.0)+vec2(float(i)*x/100.0,float(i)*y/100.0));
        if (mask.r>0.8 && mask.r -color.r<0.5){
            maskColor.rgb=  float(i)/o;
        }
        x=0.0;
        y=1.0;
        mask = texture2D(tex,gl_TexCoord[0].st+vec2(o*-x/100.0,o*-y/100.0)+vec2(float(i)*x/100.0,float(i)*y/100.0));
        if (mask.r>0.8 && mask.r -color.r<0.5){
            maskColor.rgb=  float(i)/o;
        }
        x=0.0;
        y=-1.0;
        mask = texture2D(tex,gl_TexCoord[0].st+vec2(o*-x/100.0,o*-y/100.0)+vec2(float(i)*x/100.0,float(i)*y/100.0));
        if (mask.r>0.8 && mask.r -color.r<0.5){
            maskColor.rgb=  float(i)/o;
        }
    }
}

void main(){

    kinectColor =  (blur5(tex2,gl_TexCoord[1].st,10.0));
    color =  1.10 * vec4(1.0,1.0,1.0,1.0)- kinectColor;

    mask = blur5tx(tex,gl_TexCoord[0].st,0.02);

    maskColor=vec4(0,0,0,1);


    //int offset=40;
    offsetize(10);




    vec4 infraColor= vec4(0,0,0,1);
    if (color.r<0.4){
        infraColor.b=color.r*1.0;
    }
    if (color.r>0.3 && color.r<0.6){
        infraColor.g=color.r*1.0;
    }

    if (color.r>0.5 && color.r<0.8){
        infraColor.r=color.r*1.0;
    }

    if (color.r>0.6 ){
        infraColor.r=color.r*1.0;
        infraColor.b=color.r*0.5;
        infraColor.g=color.r*0.0;
    }



    gl_FragData[0]= 0.5*infraColor+0.5*infraColor*maskColor;

    if (mask.r>0.8 && mask.r -color.r<0.25){
        gl_FragData[0].rgb=vec3(1,1,1)  ;
    }

    //gl_FragData[0]= color *3.0;
    gl_FragData[0].a= 1.0;

}
