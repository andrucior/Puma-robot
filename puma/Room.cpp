#include "Room.h"

Room::Room()
{
	glGenVertexArrays(1, &roomVAO);
	glGenBuffers(1, &roomVBO);

	glBindVertexArray(roomVAO);
	glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(room), room, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

Room::~Room()
{
	glDeleteVertexArrays(1, &roomVAO);
	glDeleteBuffers(1, &roomVBO);
}

void Room::Draw()
{
	glDisable(GL_CULL_FACE);

	glBindVertexArray(roomVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glEnable(GL_CULL_FACE);
}
