package entities;

import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.math.Vector3;

public class Player {
	Vector3 position;
	Vector3 viewDirection;
	
	Vector3 movementDirection;
	Vector2 lookDirectionAngles;
	
	float movementSpeed = 3.0f;
	float sensitivitySpeed = 0.3f;
	
	public Player(Vector3 startPosition)
	{
		this.position = startPosition;
		this.lookDirectionAngles = new Vector2(0.0f, 0.0f);
		this.viewDirection = new Vector3();
	}
	
	public void Update(float dt)
	{
		// Move position
		Vector3 deltaMovement = new Vector3(movementDirection);
		deltaMovement.z *= this.viewDirection.z;
		deltaMovement.nor();
		deltaMovement.scl(movementSpeed * dt);
		
		position.add(deltaMovement);
		
		// Look
		viewDirection.x = (float) (Math.cos(ToRad(lookDirectionAngles.x)));
		viewDirection.z = (float) (Math.sin(ToRad(lookDirectionAngles.x)));
	}
	
	double ToRad(double angle)
	{
		return Math.toRadians(angle);
	}
	
	public void SetMovementDirection(Vector3 direction) { this.movementDirection = direction; }
	public void AddLookDirection(Vector2 direction) { this.lookDirectionAngles.add(new Vector2(direction).scl(sensitivitySpeed)); }
	
	public Vector3 GetPosition() { return position; }
	public Vector3 GetViewDirection() { return viewDirection; }
}