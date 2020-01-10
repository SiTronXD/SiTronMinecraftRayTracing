package engine;

import com.badlogic.gdx.graphics.g2d.SpriteBatch;

public class GameStateManager {
	GameState currentGameState;
	
	public GameStateManager() { }
	
	public void SetGameState(GameState newGameState)
	{
		// Dispose old gamestate
		if(this.currentGameState != null)
			this.currentGameState.Dispose();
		
		this.currentGameState = newGameState;
		this.currentGameState.Init();
		this.currentGameState.LoadResources();
	}
	
	public void Update(SpriteBatch spriteBatch, float dt)
	{
		if(currentGameState != null)
		{
			currentGameState.UpdateInput();
			currentGameState.Update(dt);
			currentGameState.Render(spriteBatch);
		}
	}
	
	public void Dispose()
	{
		if(currentGameState != null)
			currentGameState.Dispose();
	}
}
