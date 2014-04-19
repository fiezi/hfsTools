uniform float time;

uniform sampler2D tex; // rendered scene
//uniform sampler2DShadow shadowTex; // rendered shadow texture
uniform sampler2D depthTex; // rendered scene texture
uniform sampler2D pickTex; //rendered picking texture
uniform sampler2D shadowTex; // rendered shadow textures

uniform mat4 lightProperties;

uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;

uniform mat4 cameraMatrix;
uniform mat4 projectionMatrix;
uniform mat4 cameraInverse;
uniform mat4 projectionInverse;

varying vec2 texCoord;

float lightDistance = 100.0;

const float specularExp = 256.0;

vec4 objectPos;

// Depth of Field variables

    vec2 tc_offset[25];

    float PI = 3.14159265358979323846264;


vec4 blur3(sampler2D myTex, vec2 tc){

      vec4 sample[9];

      float spread=1.0/400.0;//   * texture2D(myTex , tc).a/32.0;

      tc_offset[0]=spread * vec2(-1.0,-1.0);
      tc_offset[1]=spread * vec2(0.0,-1.0);
      tc_offset[2]=spread * vec2(1.0,-1.0);

      tc_offset[3]=spread * vec2(-1.0,0.0);
      tc_offset[4]=spread * vec2(0.0,0.0);
      tc_offset[5]=spread * vec2(1.0,0.0);

      tc_offset[6]=spread * vec2(-1.0,-1.0);
      tc_offset[7]=spread * vec2(0.0,-1.0);
      tc_offset[8]=spread * vec2(1.0,-1.0);

      for (int i=0 ; i<9 ; i++)
      {
        sample[i]=texture2D(myTex , tc + tc_offset[i]);
      }

      vec4 blurredColor=(
                         sample[0] + (2.0* sample[1]) + sample[2] +
                         (2.0*sample[3]) + sample[4] + (2.0*sample[5]) +
                         sample[6] + (2.0* sample[7]) + sample[8]
                        )/ 13.0;
      //blurredColor.a=1.0;
      return(blurredColor);
}


vec4 computeLight(){



    //calculate distance
    vec4 lightPos=gl_LightSource[0].position;
    lightPos.w=1.0;

    objectPos=texture2D(pickTex, texCoord);
    objectPos.w=1.0;

    vec4 distVec = lightPos - objectPos;
    float dist=distance(lightPos,objectPos);
    if (abs(dist)>gl_LightSource[0].linearAttenuation)
        return vec4(0.0,0.0,0.0,1.0);

    float linAtt = (gl_LightSource[0].linearAttenuation-abs(dist))/gl_LightSource[0].linearAttenuation;

    //transform distVec into eyespace
    vec4 lightPosEye = cameraMatrix * lightPos;
    vec4 objectPosEye = cameraMatrix * objectPos;
    vec4 distVecEye = lightPosEye - objectPosEye;

    //do Light calculations
	vec3 normal = texture2D(depthTex,texCoord).xyz;

	vec3 NN = normalize (normal);

	vec3 lightCol = gl_LightSource[0].diffuse.rgb;

    //ambient
    vec4 colorLight=0.0*texture2D(tex, texCoord);

    //diffuse
	vec3 NL = normalize( distVecEye.xyz );
	float NdotL = max(0.0,dot(NL,NN));
	colorLight.rgb += 1.0 * lightCol * NdotL;

    //specular
	if( NdotL > 0.0 ){
		vec3 NH = normalize( NL +vec3(0.0,0.0,1.0) );
        colorLight.rgb += 1.0 * lightCol * pow(max(0.0,dot(NN,NH)),specularExp);
	}


    colorLight.w=1.0;

    return colorLight * linAtt;
}

vec4 shadowMapping(){

    //gl_FragColor=texture2D(tex,texCoord);
    vec4 myLight=vec4(0.0,0.0,0.0,1.0);



   if (gl_LightSource[0].spotCutoff==0.0){
        myLight+=computeLight( );
        return myLight;
    }



    vec4 pixelPosition;
	pixelPosition=texture2D(pickTex,texCoord);
    pixelPosition.w=1.0;



    //Matrix transform to light space
    vec4 shadowCoord =   lightProjectionMatrix * lightViewMatrix * pixelPosition;

    vec4 ssShadow=shadowCoord/shadowCoord.w;
    ssShadow=ssShadow * 0.5 + 0.5;


    if (ssShadow.x<1.0 && ssShadow.x > 0.0 && ssShadow.y<1.0 && ssShadow.y >0.0){

        //this leads to hard edges. Maybe we can soften them up a bit?

            vec4 shadowColor=blur3(shadowTex, ssShadow.xy);
            float falloff = (shadowCoord.z) - shadowColor.a;
			//myLight +=max(0.0,(1.0 - falloff))	* computeLight();
			myLight+= ( min (1.0,max( 0.0,(0.25 *shadowColor.a-falloff)/(0.25*shadowColor.a) ) ) ) * computeLight( );
    }
  return myLight;
}


void main(){


      for (int i=0;i<25;i++)
            tc_offset[i]=vec2(0.0,0.0);


    gl_FragColor=texture2D(tex, texCoord);
	//gl_FragColor=texture2D(shadowTex,texCoord);
	//return;
    gl_FragColor+=shadowMapping();
	
	//gl_FragColor+=computeLight();

}
