#include <stdio.h>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    double x;
    double y;
    double z;
    double r;
    double vx;
    double vy;
    double vz;
} Circulo;

double distancia(Circulo c1, Circulo c2) {
    double dx = c1.x - c2.x;
    double dy = c1.y - c2.y;
    double dz = c1.z - c2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

void draw_sphere(Circulo c, GLUquadric* quad, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(c.x, c.y, c.z);  
    glColor3f(r, g, b);  
    gluSphere(quad, c.r, 50, 50); 
    glPopMatrix();
}

int main(int argc, char *argv[]) {
    Circulo c1, c2;  
    double T, dt;

    printf("Digite os dados da esfera 1: \n");
    printf("Posição inicial (x, y, z): \n");
    if (scanf("%lf %lf %lf", &c1.x, &c1.y, &c1.z) != 3) {
        printf("Entrada inválida!\n");
        return 1;
    }

    printf("Raio da esfera 1: \n");
    if (scanf("%lf", &c1.r) != 1 || c1.r <= 0) {
        printf("Entrada inválida!\n");
        return 1;
    }

    printf("Velocidade (vx vy vz) \n");
    if (scanf("%lf %lf %lf", &c1.vx, &c1.vy, &c1.vz) != 3 ){
        printf("Entrada inválida!\n");
        return 1;
    }

    printf("Digite os dados da esfera 2: \n");
    printf("Posição inicial (x, y, z): \n");
    if (scanf("%lf %lf %lf", &c2.x, &c2.y, &c2.z) != 3) {
        printf("Entrada inválida!\n");
        return 1;
    }

    printf("Raio da esfera 2: \n");
    if (scanf("%lf", &c2.r) != 1 || c2.r <= 0) {
        printf("Entrada inválida!\n");
        return 1;
    }

    printf("Velocidade (vx vy vz) \n");
    if (scanf("%lf %lf %lf", &c2.vx, &c2.vy, &c2.vz) != 3 ){
        printf("Entrada inválida!\n");
        return 1;
    }

    printf("Digite o tempo máximo de simulação (s): ");
    if (scanf("%lf", &T) != 1 || T <= 0) {
        printf("Entrada inválida! Tempo tem que ser positivo.\n");
        return 1;
    } 

    printf("Digite o passo de tempo (s) : ");
    if (scanf("%lf", &dt) != 1 || dt <= 0) {
        printf("Entrada inválida! Passo deve ser positivo.\n");
        return 1;
    }

    Circulo c1_atual = c1;
    Circulo c2_atual = c2;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_Window *window = SDL_CreateWindow(
        "Esferas 3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 900, SDL_WINDOW_OPENGL
    );
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("Erro ao criar contexto OpenGL: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    glClearColor(0.969f, 0.969f, 0.855f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(75.0f, 1280.0f / 900.0f, 1.0f, 1000.0f); 
    glMatrixMode(GL_MODELVIEW);

    GLUquadric* quad = gluNewQuadric();
    if (!quad) {
        printf("Erro ao criar quadric\n");
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    int running = 1;
    double tempo_simulado = 0.0;
    int colidiu = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
               (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        if (tempo_simulado <= T && !colidiu) {
            c1_atual.x += c1.vx * dt;
            c1_atual.y += c1.vy * dt;
            c1_atual.z += c1.vz * dt;

            c2_atual.x += c2.vx * dt;
            c2_atual.y += c2.vy * dt;
            c2_atual.z += c2.vz * dt;

            tempo_simulado += dt;

            double d = distancia(c1_atual, c2_atual);
            if (d <= c1.r + c2.r) {
                printf("\nColisão detetada em t = %.2lf\n", tempo_simulado);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "COLISÃO!!", "As esferas colidiram!", window);
                colidiu = 1;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(0.0f, 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        draw_sphere(c1_atual, quad, 1.0f, 0.0f, 0.0f);  
        draw_sphere(c2_atual, quad, 0.0f, 0.0f, 1.0f);

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }

    gluDeleteQuadric(quad);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

