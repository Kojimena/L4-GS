#pragma once
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include "uniforms.h"
#include "fragment.h"
#include "noise.h"
#include "print.h"

Vertex vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    // Apply transformations to the input vertex using the matrices from the uniforms
    glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

    // Perspective divide
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Apply the viewport transform
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

    // Transform the normal
    glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
    transformedNormal = glm::normalize(transformedNormal);

    glm::vec3 transformedWorldPosition = glm::vec3(uniforms.model * glm::vec4(vertex.position, 1.0f));

    // Return the transformed vertex as a vec3
    return Vertex{
            glm::vec3(screenVertex),
            transformedNormal,
            vertex.tex,
            transformedWorldPosition,
            vertex.position
    };
}


Fragment greeneShader(Fragment& fragment) {


    // Provided neon colors
    glm::vec3 neon1 = glm::vec3(0.52f, 1.0f, 0.39f);  // #360A14
    glm::vec3 neon2 = glm::vec3(0.98f, 1.0f, 0.72f);  // #961D4E
    glm::vec3 neon3 = glm::vec3(0.54f, 0.69f, 0.45f);    // #A60067
    glm::vec3 neon4 = glm::vec3(0.380f, 0.322f, 0.800f);  // #6153CC
    glm::vec3 neon5 = glm::vec3(0.267f, 1.0f, 0.820f);    // #44FFD1

    // Declare the color variable
    glm::vec3 c = neon1;  // Default color

    // Convert 3D position on sphere to 2D UV
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (4.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);

    uv = glm::clamp(uv, 0.0f, 1.0f);

// Reflejar las coordenadas UV respecto al centro para girar 180 grados
    uv = glm::vec2(1.0f - uv.x, 1.0f - uv.y);


        // Set up the noise generator
        FastNoiseLite noiseGenerator;
        noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

        float offsetX = 1000.0f;
        float offsetY = 2000.0f;
        float scale = 900.0f;

        // Genera el valor de ruido
        float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
        noiseValue = (noiseValue + 1.0f) * 0.5f;


        noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);


        c = glm::mix(c, neon2, noiseValue);

        // Blend with neon color
        c = glm::mix(c, neon2, 0.5f);

        // Apply lighting intensity
        c = c * fragment.intensity;

    // Convert glm::vec3 color to your Color class
    fragment.color = Color(c.r, c.g, c.b);

    return fragment;
}






Fragment gasShader(Fragment& fragment){
    Color color;

    glm::vec3 mainColor = glm::vec3(204.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    glm::vec3 secondColor = glm::vec3(0.0f/255.0f, 153.0f/255.0f, 153.0f/255.0f);
    glm::vec3 thirdColor = glm::vec3(204.0f/255.0f, 255.0f/255.0f, 229.0f/255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);

    // Frecuencia y amplitud de las ondas en el planeta
    float frequency = 20.0; // Ajusta la frecuencia de las líneas
    float amplitude = 0.1; // Ajusta la amplitud de las líneas

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 800.0f;
    float offsetY = 200.0f;
    float scale = 100.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.5f;

    //aplicar ruido como hacer piedras
    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    // Calcula el valor sinusoide para crear líneas
    float sinValue = glm::sin(uv.y * frequency) * amplitude;

    // Combina el color base con las líneas sinusoide
    secondColor = secondColor + glm::vec3 (sinValue);

    color = Color(secondColor.x, secondColor.y, secondColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment sunShader (Fragment& fragment){
    Color color;

    glm::vec3 mainColor = glm::vec3(113.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f);
    glm::vec3 secondColor = glm::vec3(238.0f/255.0f, 186.0f/255.0f, 11.0f/255.0f);
    glm::vec3 thirdColor = glm::vec3(195.0f/255.0f, 111.0f/255.0f, 9.0f/255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);


    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 8000.0f;
    float offsetY = 10000.0f;
    float scale = 1000.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.5f;

    //aplicar un resplandor a las orillas de la esfera
    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    if (noiseValue > 0.2f){
        // Calcula el valor sinusoide para crear líneas
        float sinValue = glm::sin(uv.y * 20.0f) * 0.1f;

        sinValue = glm::smoothstep(0.8f, 1.0f, sinValue);

        // Combina el color base con las líneas sinusoide
        secondColor = secondColor + glm::vec3 (sinValue);
    }


    // Interpola entre el color base y el color secundario basado en el valor de ruido
    thirdColor = glm::mix(secondColor, thirdColor, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    mainColor = glm::mix(thirdColor, mainColor, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    color = Color(secondColor.x, secondColor.y, secondColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;

}

Fragment landShader(Fragment& fragment){
    // Define the colors
    glm::vec3 spotColorGreen = glm::vec3(0.133f, 0.545f, 0.133f);  // Forest green
    glm::vec3 spotColorBlue = glm::vec3(0.0f, 0.0f, 1.0f);  // Blue
    glm::vec3 earthColorLightGreen = glm::vec3(0.0f, 0.32f, 0.22f);
    glm::vec3 earthColorBrown = glm::vec3(0.49f, 0.32f, 0.22f);  // Brown for earth
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);  // White

    // Convert 3D position on sphere to 2D UV
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (4.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);

    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Set up the noise generator
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 100.0f;
    float oy = 200000.0f;
    float z = 600.0f;

    // Generate the noise value
    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    glm::vec3 c;

    if (noiseValue < 0.4f) {
        // Water
        c = glm::mix(spotColorBlue, earthColorLightGreen, noiseValue);
        c = glm::smoothstep(glm::vec3(0.0f), glm::vec3(0.9f), c);
    } else if (noiseValue < 0.6f) {
        // Earth/Brown
        c = earthColorLightGreen;
    } else if (noiseValue < 0.8f) {
        // Land/Green
        c = spotColorGreen;
    } else if (noiseValue < 0.9f) {
        // Land/Green
        c = earthColorBrown;
    } else {
        //dejar azul
        c = spotColorBlue;
    }

    float cloudNoise = noiseGenerator.GetNoise((uv.x + ox) * z * 0.5f, (uv.y + oy) * z * 0.5f);
    cloudNoise = (cloudNoise + 1.0f) * 0.3f;
    cloudNoise = glm::smoothstep(0.0f, 1.0f, cloudNoise);

    // Interpolate between the cloud color and the planet color based on the cloud noise
    c = glm::mix(c, cloudColor, cloudNoise);


    // Convert glm::vec3 color to your Color class
    fragment.color = Color(c.r, c.g, c.b);

    return fragment;
}


Fragment neonShader(Fragment& fragment) {

    Color color;

    glm::vec3 mainColor = glm::vec3(145.0f/255.0f, 179.0f/255.0f, 225.0f/255.0f);
    glm::vec3 secondColor = glm::vec3(175.0f/255.0f, 0.0f/255.0f, 225.0f/255.0f);
    glm::vec3 thirdColor = glm::vec3(175.0f/255.0f, 255.0f/255.0f, 80.0f/255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);


    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 15000.0f;
    float offsetY = 400.0f;
    float scale = 250.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.9f;

    //aplicar un resplandor a las orillas de la esfera
    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    if (noiseValue > 0.5f){
        // hacer que las orillas sean mas brillantes
        secondColor = secondColor + glm::vec3 (0.1f);

        // Calcula el valor sinusoide para crear líneas
        float sinValue = glm::sin(uv.y * 20.0f) * 0.1f;

        sinValue = glm::smoothstep(0.8f, 1.0f, sinValue);

    }else{
        // Calcula el valor sinusoide para crear líneas
        float sinValue = glm::sin(uv.y * 50.0f) * 0.1f;

        sinValue = glm::smoothstep(0.1f, 1.0f, sinValue);

        // Combina el color base con las líneas sinusoide
        secondColor = secondColor + glm::vec3 (sinValue);
    }


    // Interpola entre el color base y el color secundario basado en el valor de ruido
    mainColor = glm::mix(thirdColor, mainColor, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    color = Color(secondColor.x, secondColor.y, secondColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}


Fragment ballShader(Fragment& fragment){
    // Define the colors
    glm::vec3 spotColorGreen = glm::vec3(0.133f, 0.545f, 0.133f);  // Forest green
    glm::vec3 spotColorBlue = glm::vec3(0.0f, 0.0f, 1.0f);  // Blue
    glm::vec3 earthColorLightGreen = glm::vec3(0.0f, 0.32f, 0.22f);
    glm::vec3 earthColorBrown = glm::vec3(0.49f, 0.32f, 0.22f);  // Brown for earth
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);  // White

    // Provided neon colors
    glm::vec3 neon1 = glm::vec3(0.211f, 0.039f, 0.078f);  // #360A14
    glm::vec3 neon2 = glm::vec3(0.588f, 0.114f, 0.306f);  // #961D4E
    glm::vec3 neon3 = glm::vec3(0.651f, 0.0f, 0.404f);    // #A60067
    glm::vec3 neon4 = glm::vec3(0.380f, 0.322f, 0.800f);  // #6153CC
    glm::vec3 neon5 = glm::vec3(0.267f, 1.0f, 0.820f);    // #44FFD1

    // Convert 3D position on sphere to 2D UV
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (4.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);

    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Neon circles
    float distToCenter = glm::distance(uv, glm::vec2(0.5f, 0.5f));
    glm::vec3 neonColor;
    if (distToCenter < 0.1f) {
        neonColor = neon1;
    } else if (distToCenter < 0.2f) {
        neonColor = neon2;
    } else if (distToCenter < 0.3f) {
        neonColor = neon3;
    } else if (distToCenter < 0.4f) {
        neonColor = neon4;
    } else if (distToCenter < 0.5f) {
        neonColor = neon5;
    } else {
        neonColor = glm::vec3(0.0f);
    }

    // Set up the noise generator
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 100.0f;
    float oy = 200000.0f;
    float z = 200.0f;

    // Generate the noise value
    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    glm::vec3 c;

    c = neonColor;

    float cloudNoise = noiseGenerator.GetNoise((uv.x + ox) * z * 0.5f, (uv.y + oy) * z * 0.5f);
    cloudNoise = (cloudNoise + 1.0f) * 0.3f;
    cloudNoise = glm::smoothstep(0.0f, 1.0f, cloudNoise);

    // Interpolate between the cloud color and the planet color based on the cloud noise
    c = glm::mix(c, cloudColor, cloudNoise);

    // Blend with neon color
    c = glm::mix(c, neonColor, 0.5f);

    // Convert glm::vec3 color to your Color class
    fragment.color = Color(c.r, c.g, c.b);

    return fragment;
}

Fragment starShaders(Fragment& fragment){
    Color color;

    glm::vec3 secondColor = glm::vec3(0.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f);
    glm::vec3 mainColor = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 8000.0f;
    float offsetY = 1000.0f;
    float scale = 30000.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.9f;

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    if (noiseValue > 0.99f){
        // Calcula el valor sinusoide para crear líneas
        float sinValue = glm::sin(uv.y * 20.0f) * 0.1f;

        sinValue = glm::smoothstep(0.8f, 1.0f, sinValue);

        // Combina el color base con las líneas sinusoide
        secondColor = secondColor + glm::vec3 (sinValue);
    }


    // Interpola entre el color base y el color secundario basado en el valor de ruido
    mainColor = glm::mix(mainColor, mainColor, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    color = Color(secondColor.x, secondColor.y, secondColor.z);

    fragment.color = color;

    return fragment;
}





