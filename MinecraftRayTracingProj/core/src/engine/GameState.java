package engine;

import com.badlogic.gdx.graphics.g2d.SpriteBatch;

public abstract class GameState {
	GameStateManager gsm;
	
	public GameState(GameStateManager gsm)
	{
		this.gsm = gsm;
	}
	
	public abstract void Init();
	public abstract void LoadResources();
	public abstract void UpdateInput();
	public abstract void Update(float dt);
	public abstract void Render(SpriteBatch spriteBatch);
	public abstract void Dispose();
}
