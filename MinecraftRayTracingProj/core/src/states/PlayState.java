package states;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.ShaderProgram;

import engine.GameState;
import engine.GameStateManager;

public class PlayState extends GameState {
	Texture screenTexture;
	
	SpriteBatch rayTracingSpriteBatch;
	
	ShaderProgram shaderProgram;
	
	@Override
	public void Init() {
		// Load shaders
		FileHandle vertexShader = Gdx.files.internal("shaders/RayTracingVertexShader.glsl");
		FileHandle fragmentShader = Gdx.files.internal("shaders/RayTracingFragmentShader.glsl");

		shaderProgram = new ShaderProgram(vertexShader, fragmentShader);
		ShaderProgram.pedantic = false;
		
		// Debug compile
		System.out.println(shaderProgram.isCompiled() ? "Ray Tracing Shader Was Compiled!" : shaderProgram.getLog());
		
		// Custom sprite batch only made for the ray tracer
		rayTracingSpriteBatch = new SpriteBatch();
		rayTracingSpriteBatch.setShader(shaderProgram);
	}
	
	@Override
	public void LoadResources()
	{
		screenTexture = new Texture("gfx/BenisTexture.png");
	}

	@Override
	public void UpdateInput() {

	}

	@Override
	public void Update(float dt) {

	}

	@Override
	public void Render(SpriteBatch spriteBatch) {
		shaderProgram.begin();
		
		
		// Shader uniforms
		shaderProgram.setUniformMatrix("viewProjectionMatrix", rayTracingSpriteBatch.getProjectionMatrix());
		
		
		rayTracingSpriteBatch.begin();
		
		
		// Draw
		rayTracingSpriteBatch.draw(screenTexture, 0, 0, Gdx.graphics.getWidth(), Gdx.graphics.getHeight());
		
		
		rayTracingSpriteBatch.end();
		shaderProgram.end();
	}

	@Override
	public void Dispose() {
		rayTracingSpriteBatch.dispose();
		shaderProgram.dispose();
		screenTexture.dispose();
	}

	public PlayState(GameStateManager gsm) { super(gsm); }
}
