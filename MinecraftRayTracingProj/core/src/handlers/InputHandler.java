package handlers;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.math.Vector3;

import entities.Player;

public class InputHandler {
	Vector2 lastMousePos;
	
	Player player;
	
	public InputHandler(Player player)
	{
		this.player = player;
		
		lastMousePos = new Vector2();
	}
	
	public void UpdateInput()
	{
		// Movement input
		int forward = Gdx.input.isKeyPressed(Keys.W) ? 1 : 0;
		int back = Gdx.input.isKeyPressed(Keys.S) ? 1 : 0;
		int left = Gdx.input.isKeyPressed(Keys.A) ? 1 : 0;
		int right = Gdx.input.isKeyPressed(Keys.D) ? 1 : 0;
		int up = Gdx.input.isKeyPressed(Keys.E) ? 1 : 0;
		int down = Gdx.input.isKeyPressed(Keys.Q) ? 1 : 0;
		
		// Look input
		Vector2 deltaMouseMovement = new Vector2(Gdx.input.getX(), Gdx.input.getY()).sub(new Vector2(lastMousePos));
		lastMousePos = new Vector2(Gdx.input.getX(), Gdx.input.getY());
		
		// Apply
		player.SetMovementDirection(new Vector3(right - left, up - down, forward - back));
		player.AddLookDirection(deltaMouseMovement);
	}
}
