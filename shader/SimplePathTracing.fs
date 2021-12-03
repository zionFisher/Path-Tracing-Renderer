#version 330 core

// Variables-------------------------------------------------------------------
#define EPSILON 0.0001                         // Float EPSILON
#define PI      3.1415926535897                // PI

in vec3 rayDirection;                          // Ray Direction
in vec3 eye;                                   // Position of eye
in vec2 screen;                                // Width and Height of Screen(window actually)

out vec4 FragColor;                            // Output Color

uniform int        spp;                        // Samples Per Pixel
uniform float[4]   rdSeed;                     // Random seed
uniform float[416] Triangles;                  // 32 Triangles
uniform float      RussianRoulette;            // Russian Roulette
uniform float      IndirLightContriRate;       // Indirect Light Contribution Rate
uniform mat4       RayRotateMatrix;

float rdCount;                                 // Random counter
float pdfLight;                                // PDF of light
vec3  debugger   = vec3(1.0, 1.0, 1.0);        // Only for debug(it's too hard to debug in GLSL)
vec3  lightColor = vec3(1.0, 1.0, 1.0);        // Default light color

// Struct----------------------------------------------------------------------
struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Triangle
{
    vec3 v0;
    vec3 v1;
    vec3 v2;
    // vec3 normal;
    vec3 Kd;
    // vec3 Ks
    bool isLight;
};

struct Intersection
{
    bool happened; // isIntersect
    bool isLight;
    vec3 coords;
    vec3 normal;
    vec3 Kd;
    // vec3 Ks
    float distance;
};

// Declaration-----------------------------------------------------------------

// Main
void main();

// Shading
vec3 Shade (Ray ray);

// Random
float RandXY       (float x, float y);
float Rand         ();
float GetRandFloat ();

// BRDF
vec3 BRDF (vec3 wi, vec3 wo, vec3 N, vec3 Kd);

// Intersection
Intersection IntersectTriangle (Ray ray, Triangle triangle);
Intersection IntersectScene    (Ray ray);

// Triangle Process
float        GetTriangleArea     (Triangle triangle);
float        PDFTriangle         (vec3 wi, vec3 wo, vec3 N);
vec3         SampleTriangle      (vec3 wi, vec3 N);
Intersection SampleTriangleLight (Triangle triangle);
Intersection SampleLight         ();

// Main------------------------------------------------------------------------
void main()
{
	vec3 color;

    vec4 rayDir = RayRotateMatrix * vec4(rayDirection, 0.0f);

	color = Shade(Ray(eye, vec3(rayDir.x, rayDir.y, rayDir.z)));

    // color = vec3(Rand());

	FragColor = vec4(color, 1.0);
}

// Shading---------------------------------------------------------------------
vec3 Shade(Ray ray)
{
    // Special case: inside the scene or is a light.
	Intersection scene = IntersectScene(ray);

    if (scene.happened == false)
		return vec3(0.0, 0.0, 0.0);

    if (scene.isLight)
        return lightColor;  // default light color

    // Iteration Implementation
    vec3 colorBuffer[20];

    int dirLightIndex = 0, indirLightIndex = 19;

    vec3 emit = 8.0f  * vec3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) +
                15.6f * vec3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) +
                18.4f * vec3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
    emit *= 2;

    vec3 color = vec3(0.0f);

    int counter = 0;

    for (int i = 0; i < spp; ++i)
    {
        vec3 result = vec3(0.0f);

        bool flag = true;

        Ray curRay = ray;

        while (flag)
        {
            Intersection inter = IntersectScene(curRay);

	        vec3 dirLight = vec3(0.0f, 0.0f, 0.0f);

            int depth = 0;

            vec3 p = inter.coords;
            vec3 N = normalize(inter.normal);
            vec3 wo = normalize(-ray.direction);

            Intersection interLight = SampleLight();

            vec3 x = interLight.coords;
            vec3 ws = normalize(x - p);
            vec3 NN = normalize(interLight.normal);

            bool block = length(IntersectScene(Ray(p, ws)).coords - x) > EPSILON;

            if (!block)
            {
                colorBuffer[dirLightIndex++] = (emit * BRDF(wo, ws, N, inter.Kd) * dot(ws, N) * dot(-ws, NN))
                                               /
                                               ((length(x - p) * length(x - p)) * pdfLight);
            }

            float seed = GetRandFloat();
            if (seed >= RussianRoulette || indirLightIndex - dirLightIndex <= 2)
            {
                colorBuffer[indirLightIndex--] = vec3(0.0f);
                break;
            }

            // pass Ruaaian Roulette test.
            vec3 wi = normalize(SampleTriangle(wo, N));
            Ray reflectRay = Ray(p, wi);
            Intersection reflectInter = IntersectScene(reflectRay);
            if (reflectInter.happened && !reflectInter.isLight)
            {
                colorBuffer[indirLightIndex--] = IndirLightContriRate * BRDF(wo, wi, N, inter.Kd) * dot(wi, N)
                                                 /
                                                 (PDFTriangle(wo, wi, N) * RussianRoulette);
            }

            curRay = reflectRay;
            flag = reflectInter.happened;
        }

        for (int i = dirLightIndex; i >= 0; i--)
        {
            result += colorBuffer[i] + colorBuffer[19 - i] * result;
        }

        color += result / spp;
        counter++;
    }

	return color;
}

// Random----------------------------------------------------------------------
float RandXY(float x, float y)
{
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt = dot(vec2(x, y) ,vec2(a,b));
    float sn = mod(dt,3.14);
    return fract(sin(sn) * c);
}

// // optional
// float getNoise(float x, float y)
// {
//     vec2 theta_factor_a = vec2(0.9898, 0.233);
//     vec2 theta_factor_b = vec2(12.0, 78.0);
//
//     float theta_a = dot(vec2(x, y), theta_factor_a);
//     float theta_b = dot(vec2(x, y), theta_factor_b);
//     float theta_c = dot(vec2(y, x), theta_factor_a);
//     float theta_d = dot(vec2(y, x), theta_factor_b);
//
//     float value = cos(theta_a) * sin(theta_b) + sin(theta_c) * cos(theta_d);
//     float temp = mod(197.0 * value, 1.0) + value;
//     float part_a = mod(220.0 * temp, 1.0) + temp;
//     float part_b = value * 0.5453;
//     float part_c = cos(theta_a + theta_b) * 0.43758;
//
//     return fract(part_a + part_b + part_c);
// }

// [0.0f, 1.0f)
float Rand()
{
    float a = RandXY(rayDirection.x, rdSeed[0]);
    float b = RandXY(rdSeed[1], rayDirection.y);
    float c = RandXY(rdCount++, rdSeed[2]);
    float d = RandXY(rdSeed[3], a);
    float e = RandXY(b, c);
    float f = RandXY(d, e);

    return f;
}

// 0 ~ 1
float GetRandFloat()
{
    return Rand();
}

// BRDF------------------------------------------------------------------------
// Currently only diffuse is supported, so wi have been never used.
vec3 BRDF(vec3 wi, vec3 wo, vec3 N, vec3 Kd)
{
	float cosalpha = dot(N, wo);

	if (cosalpha > 0.0f)
	{
	     vec3 diffuse = Kd / PI;
         return diffuse;
	}
    else return vec3(0.0f);
}

// Intersection----------------------------------------------------------------
Intersection IntersectTriangle(Ray ray, Triangle triangle)
{
    Intersection inter;
	inter.happened = false;

	vec3 e1 = triangle.v1 - triangle.v0;
    vec3 e2 = triangle.v2 - triangle.v0;

    vec3 triangleNormal = normalize(cross(e1, e2));
    if (dot(ray.direction, triangleNormal) > 0)
        return inter;

    vec3 pvec = cross(ray.direction, e2);
    float det = dot(e1, pvec);
    if (abs(det) < EPSILON)
        return inter;

    float det_inv = 1.0 / det;
    vec3 tvec = ray.origin - triangle.v0;
    float u = dot(tvec, pvec) * det_inv;
    if (u < 0 || u > 1)
        return inter;

    vec3 qvec = cross(tvec, e1);
    float v = dot(ray.direction, qvec) * det_inv;
    if (v < 0 || u + v > 1)
        return inter;

    float t_tmp = dot(e2, qvec) * det_inv;
    if (t_tmp < 0)
        return inter;

    inter.happened = true;
    inter.coords = ray.origin + ray.direction * t_tmp;
    inter.normal = triangleNormal;
    inter.distance = t_tmp;
    inter.Kd = triangle.Kd;
    inter.isLight = triangle.isLight;

    return inter;
}

Intersection IntersectScene(Ray ray)
{
	Intersection inter, temp;
	inter.happened = false;

	float minDistance = -1;

	int triangleIndex = 0;

	for (int i = 0; i < 32; ++i)
    {
        Triangle triangle = Triangle(vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     abs(Triangles[triangleIndex++] - 1.0f) < EPSILON);

        temp = IntersectTriangle(ray, triangle);
        if (temp.happened && (temp.distance <= minDistance || minDistance < 0))
		{
			inter = temp;
			minDistance = temp.distance;
		}
	}

	return inter;
}

// Triangle Process------------------------------------------------------------
float GetTriangleArea(Triangle triangle)
{
    return length(cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0)) * 0.5;
}

float PDFTriangle(vec3 wi, vec3 wo, vec3 N)
{
    if (dot(wo, N) > 0.0f)
        return 0.5f * PI;
    else
        return 0.0f;
}

vec3 SampleTriangle(vec3 wi, vec3 N)
{
    float x1 = GetRandFloat(), x2 = GetRandFloat();
    float z = abs(1.0f - 2.0f * x1);
    float r = sqrt(1.0f - z * z), phi = 2 * PI * x2;
    vec3 localRay = vec3(r * cos(phi), r * sin(phi), z);

    vec3 B, C;
    if (abs(N.x) > abs(N.y))
    {
        float invLen = 1.0f / sqrt(N.x * N.x + N.z * N.z);
        C = vec3(N.z * invLen, 0.0f, -N.x * invLen);
    }
    else
    {
        float invLen = 1.0f / sqrt(N.y * N.y + N.z * N.z);
        C = vec3(0.0f, N.z * invLen, -N.y * invLen);
    }
    B = cross(C, N);

    return localRay.x * B + localRay.y * C + localRay.z * N;
}

Intersection SampleTriangleLight(Triangle triangle)
{
    Intersection inter;
    float x = sqrt(GetRandFloat());
    float y = GetRandFloat();

    inter.coords = triangle.v0 * (1.0f - x) + triangle.v1 * (x * (1.0f - y)) + triangle.v2 * (x * y);
    inter.normal = normalize(cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0));
    pdfLight += 1.0f / GetTriangleArea(triangle);
    return inter;
}

Intersection SampleLight()
{
    Intersection inter;
    float emitAreaSum = 0;
    int triangleIndex = 0;

    for (int i = 0; i < 32; ++i)
    {
        Triangle triangle = Triangle(vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     abs(Triangles[triangleIndex++] - 1.0f) < EPSILON);
        if (triangle.isLight)
        {
            emitAreaSum += GetTriangleArea(triangle);
        }
    }

    float p = GetRandFloat() * emitAreaSum;
    emitAreaSum = 0;
    triangleIndex = 0;

    for (int i = 0; i < 32; ++i)
    {
        Triangle triangle = Triangle(vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     vec3(Triangles[triangleIndex++], Triangles[triangleIndex++], Triangles[triangleIndex++]),
                                     abs(Triangles[triangleIndex++] - 1.0f) < EPSILON);
        if (triangle.isLight)
        {
            emitAreaSum += GetTriangleArea(triangle);
            if (p <= emitAreaSum)
            {
                inter = SampleTriangleLight(triangle);
                break;
            }
        }
    }

    return inter;
}