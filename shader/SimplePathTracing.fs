#version 330 core
#define EPSILON 0.0001
#define PI      3.1415926535897

in vec3 rayDirection;
in vec3 origin;
in vec2 screen;

out vec4 FragColor;

uniform float time;
uniform int spp;
uniform float[4] rdSeed;
uniform float[416] Triangles;
uniform float RussianRoulette;
uniform float IndirLightContributionRate;

float rdCnt;

vec3 debugger = vec3(1.0, 1.0, 1.0);
float pdf_light;
vec3 lightColor = vec3(1.0, 1.0, 1.0);

highp float randXY(float x, float y)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt = dot(vec2(x, y) ,vec2(a,b));
    highp float sn = mod(dt,3.14);
    return fract(sin(sn) * c);
}

// [0.0f, 1.0f)
float rand()
{
    float a = randXY(rayDirection.x, rdSeed[0]);
    float b = randXY(rdSeed[1], rayDirection.y);
    float c = randXY(rdCnt++, rdSeed[2]);
    float d = randXY(rdSeed[3], a);
    float e = randXY(b, c);
    float f = randXY(d, e);

    return f;
}

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
    bool isLight;
};

struct Intersection
{
    bool happened; // isIntersect
    bool isLight;
    vec3 coords;
    vec3 normal;
    vec3 Kd;
	vec3 emit;
    float distance;
};

// 0 ~ 1
float getRandFloat()
{
    return rand();
}

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

// Currently only diffuse is supported right now, so wi have been never used.
vec3 eval(vec3 wi, vec3 wo, vec3 N, vec3 Kd)
{
	float cosalpha = dot(N, wo);

	if (cosalpha > 0.0f)
	{
	     vec3 diffuse = Kd / PI;
         return diffuse;
	}
    else return vec3(0.0f);
}

float getTriangleArea(Triangle triangle)
{
    return length(cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0)) * 0.5;
}

vec3 sampleTriangle(vec3 wi, vec3 N)
{
    float x1 = getRandFloat(), x2 = getRandFloat();
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

float pdfTriangle(vec3 wi, vec3 wo, vec3 N)
{
    if (dot(wo, N) > 0.0f)
        return 0.5f * PI;
    else
        return 0.0f;
}

Intersection sampleTriangleLight(Triangle triangle)
{
    Intersection inter;
    float x = sqrt(getRandFloat());
    float y = getRandFloat();

    inter.coords = triangle.v0 * (1.0f - x) + triangle.v1 * (x * (1.0f - y)) + triangle.v2 * (x * y);
    inter.normal = normalize(cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0));
    pdf_light += 1.0f / getTriangleArea(triangle);
    return inter;
}

Intersection sampleLight()
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
            emitAreaSum += getTriangleArea(triangle);
        }
    }

    float p = getRandFloat() * emitAreaSum;
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
            emitAreaSum += getTriangleArea(triangle);
            if (p <= emitAreaSum)
            {
                inter = sampleTriangleLight(triangle);
                break;
            }
        }
    }

    return inter;
}

float norm(vec3 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

vec3 castRay(Ray ray)
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
    emit = emit * 2;

    vec3 color = vec3(0.0f);

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

            Intersection interLight = sampleLight();

            vec3 x = interLight.coords;
            vec3 ws = normalize(x - p);
            vec3 NN = normalize(interLight.normal);

            bool block = length(IntersectScene(Ray(p, ws)).coords - x) > EPSILON;

            if (!block)
            {
                colorBuffer[dirLightIndex++] = (emit * eval(wo, ws, N, inter.Kd) * dot(ws, N) * dot(-ws, NN))
                                               /
                                               ((norm(x - p) * norm(x - p)) * pdf_light);
            }

            float seed = getRandFloat();
            if (seed >= RussianRoulette || indirLightIndex - dirLightIndex <= 2)
            {
                colorBuffer[indirLightIndex--] = vec3(0.0f);
                break;
            }

            // pass Ruaaian Roulette test.
            vec3 wi = normalize(sampleTriangle(wo, N));
            Ray reflectRay = Ray(p, wi);
            Intersection reflectInter = IntersectScene(reflectRay);
            if (reflectInter.happened && !reflectInter.isLight)
            {
                colorBuffer[indirLightIndex--] = IndirLightContributionRate * eval(wo, wi, N, inter.Kd) * dot(wi, N)
                                                 /
                                                 (pdfTriangle(wo, wi, N) * RussianRoulette);
            }

            curRay = reflectRay;
            flag = reflectInter.happened;
        }

        for (int i = dirLightIndex; i >= 0; i--)
        {
            result += colorBuffer[i] + colorBuffer[19 - i] * result;
        }

        color += result / spp;
    }

	return color;
}

void main()
{
	vec3 color;

	color = castRay(Ray(origin, rayDirection));

    // color = vec3(rand());

	FragColor = vec4(color, 1.0);
}