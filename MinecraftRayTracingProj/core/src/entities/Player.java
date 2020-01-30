package entities;

import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.math.Vector3;

public class Player {
	Vector3 position;
	Vector3 viewDirection;
	Vector3 movementDirection;
	
	float movementSpeed = 3.0f;
	
	public Player(Vector3 startPosition)
	{
		this.position = startPosition;
		this.viewDirection = new Vector3(0.0f, 0.0f, 1.0f);
	}
	
	public void Update(float dt)
	{
		// Move
		Vector3 deltaMovement = new Vector3(movementDirection);
		deltaMovement.nor();
		deltaMovement.scl(movementSpeed * dt);
		
		position.add(deltaMovement);
	}
	
	public void SetMovementDirection(Vector3 direction) { this.movementDirection = direction; }
	
	public Vector3 GetPosition() { return position; }
	public Vector3 GetViewDirection() { return viewDirection; }
}