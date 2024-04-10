#include <iostream>

#include "application.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

int main()
{
    InitWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Memory Freelist Visualiser" );
    SetTargetFPS( 60 );

	Application application( 
		Rectangle {
			0.0f, 0.0f,
			WINDOW_WIDTH, WINDOW_HEIGHT
		}
	);

	while ( !WindowShouldClose() )
	{
		application.update( GetFrameTime() );

		BeginDrawing();
		application.render();
		EndDrawing();
	}

    CloseWindow();
    return 0;
}