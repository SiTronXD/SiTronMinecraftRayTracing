package game;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

import engine.GameStateManager;
import states.PlayState;

public class MainGame extends ApplicationAdapter {
	SpriteBatch spriteBatch;
	Texture img;
	
	GameStateManager gsm;
	
	@Override
	public void create () {
		spriteBatch = new SpriteBatch();
		img = new Texture("gfx/minecraftTextureSheet.png");
		
		gsm = new GameStateManager();
		gsm.SetGameState(new PlayState(gsm));
	}

	@Override
	public void render () {
		Gdx.gl.glClearColor(0.2f, 0.2f, 0.2f, 1);
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
		
		gsm.Update(spriteBatch, Gdx.graphics.getDeltaTime());
		
		// DEBUGGING!!!!!!!
		if(Gdx.input.isKeyJustPressed(Keys.ESCAPE))
			Gdx.app.exit();
	}
	
	@Override
	public void dispose () {
		gsm.Dispose();
		
		spriteBatch.dispose();
		img.dispose();
	}
}
