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

Fragment fragmentShaderStripes(Fragment& fragment) {
    // Define the base color for Mars

    // Create some noise based on the fragment position
    /* float noise = glm::fract(sin(glm::dot(glm::vec2(fragment.x, fragment.y), glm::vec2(12.9898f, 78.233f))) * 43758.5453f); */

    /* float noise = glm::fract(sin(glm::dot(fragment.worldPos, glm::vec3(12.9898f, 78.233f, 56.789f))) * 43758.5453f); */
    // Slightly adjust the color based on the noise
    /* Color color = marsColor + Color(static_cast<int>(noise * 25), static_cast<int>(noise * 10), static_cast<int>(noise * 5)); */
    /* glm::vec3 wc = fragment.worldPos * 1.0f + 0.5f; */
    /* Color color = Color(wc.r, wc.g, wc.b); */

    // Define a color for Jupiter. You may want to adjust this to better match Jupiter's appearance.
    Color baseColor = Color(125, 67, 37);  // This color is a kind of dark orange

    // Create stripes based on the fragment's Y position in world space. The frequency of the stripes
    // can be controlled by scaling the Y coordinate.
    // The number 10.0f determines the frequency of the stripes, adjust it to get the desired effect.
    float stripePattern = glm::abs(glm::cos(fragment.originalPos.y * 20.0f));

    // Interpolate between the base color and a darker version of the base color based on the stripe pattern.
    // This will create dark stripes on the sphere.
    Color stripeColor = baseColor * (0.5f + 0.5f * stripePattern);

    // Apply lighting intensity
    stripeColor = stripeColor * fragment.intensity;

    // Set the fragment color
    Color color = stripeColor;



    // Define the direction to the center of the circle in world space
    // Apply lighting intensity
    color = color * fragment.intensity;

    // Set the fragment color
    fragment.color = color;

    return fragment;
}

Fragment fragmentShaderNorthPole(Fragment& fragment) {
    // Define a color for the ocean and the continent
    glm::vec3 oceanColor = glm::vec3(0, 0, 128);  // Dark blue
    glm::vec3 continentColor = glm::vec3(34, 139, 34);  // Forest green

    // Create a simple "continent" in the northern hemisphere by making a region where the Y coordinate is
    // above a certain value look different
    float continentPattern = glm::smoothstep(0.4f, 1.0f, fragment.originalPos.y);

    // Interpolate between the ocean color and the continent color based on the continent pattern
    glm::vec3 c = glm::mix(oceanColor, continentColor, continentPattern);
    Color color = Color(c.x, c.y, c.z);
    // Apply lighting intensity
    color = color * fragment.intensity;

    // Set the fragment color
    fragment.color = color;

    return fragment;
}

Fragment fragmentShaderGreeN(Fragment& fragment) {
    // Define the color for the ocean and the continent using integer values
    Color oceanColor = Color(0, 0, 128);  // Dark blue
    Color continentColor = Color(34, 139, 34);  // Forest green

    // Create a simple "continent" using a spot on the side of the planet.
    // The spot is defined as a region where the x coordinate is within a certain range.
    float continentPattern = glm::smoothstep(-0.2f, 0.2f, fragment.originalPos.x);

    // Interpolate between the ocean color and the continent color based on the continent pattern
    Color c = oceanColor * (1.0f - continentPattern) + continentColor * continentPattern;

    // Apply lighting intensity
    c = c * fragment.intensity;

    // Set the fragment color
    fragment.color = c;

    return fragment;
}

Fragment fragmentShaderSun(Fragment& fragment) {
    // Color base para el sol: un amarillo-anaranjado brillante
    glm::vec3 baseColor = glm::vec3(1.0f, 0.8f, 0.2f);

    // Usar Perlin noise para obtener variaciones en el color
    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.z);
    uv = glm::clamp(uv, 0.0f, 1.0f);  // Asegurarse de que las coordenadas uv estén en el rango [0, 1]

    // Configurar el generador de ruido
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    float noiseValue = noiseGenerator.GetNoise(uv.x * 10.0f, uv.y * 10.0f);

    // Usar el valor de ruido para mezclar entre varios colores
    glm::vec3 noiseColor = glm::mix(glm::vec3(1.0f, 0.5f, 0.3f),  // naranja-rojizo
                                    glm::vec3(1.0f, 0.3f, 0.5f),  // rosa
                                    noiseValue);

    glm::vec3 finalColor = glm::mix(baseColor, noiseColor, 0.5f);

    // Convertir glm::vec3 color a tu clase Color
    fragment.color = Color(finalColor.r * 255, finalColor.g * 255, finalColor.b * 255);

    return fragment;
}

Fragment fragmentShader(Fragment& fragment) {

    Color color;

    glm::vec3 mainColor = glm::vec3(145.0f/255.0f, 179.0f/255.0f, 225.0f/255.0f);
    glm::vec3 secondColor = glm::vec3(175.0f/255.0f, 0.0f/255.0f, 225.0f/255.0f);
    glm::vec3 thirdColor = glm::vec3(175.0f/255.0f, 255.0f/255.0f, 80.0f/255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);


    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 15000.0f;
    float offsetY = 400.0f;
    float scale = 800.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.9f;

    //aplicar un resplandor a las orillas de la esfera
    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    if (noiseValue > 0.5f){
        // Calcula el valor sinusoide para crear líneas
        float sinValue = glm::sin(uv.y * 20.0f) * 0.1f;

        sinValue = glm::smoothstep(0.8f, 1.0f, sinValue);

        // Combina el color base con las líneas sinusoide
        secondColor = secondColor + glm::vec3 (sinValue);
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





