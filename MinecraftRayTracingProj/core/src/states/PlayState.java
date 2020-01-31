package states;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.ShaderProgram;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.math.Vector3;

import engine.GameState;
import engine.GameStateManager;
import entities.Player;
import handlers.InputHandler;

public class PlayState extends GameState {
	Texture screenTexture;
	
	SpriteBatch rayTracingSpriteBatch;
	
	ShaderProgram shaderProgram;
	
	InputHandler inputHandler;
	
	Player player;
	
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
		
		player = new Player(new Vector3(0.0f, 0.0f, 5.0f));
		
		inputHandler = new InputHandler(player);
	}
	
	@Override
	public void LoadResources()
	{
		screenTexture = new Texture("gfx/BenisTexture.png");
	}

	@Override
	public void UpdateInput() {
		inputHandler.UpdateInput();
	}

	@Override
	public void Update(float dt) {
		player.Update(dt);
	}

	@Override
	public void Render(SpriteBatch spriteBatch) {
		shaderProgram.begin();
		
		// Shader uniforms
		shaderProgram.setUniformMatrix("viewProjectionMatrix", rayTracingSpriteBatch.getProjectionMatrix());
		shaderProgram.setUniform2fv("resolution", new float[] { 1920f, 1080f }, 0, 2);
		
		float[] cameraPosition = new float[3]; 
		cameraPosition[0] = player.GetPosition().x;
		cameraPosition[1] = player.GetPosition().y;
		cameraPosition[2] = player.GetPosition().z;
		shaderProgram.setUniform3fv("u_cameraPosition", cameraPosition, 0, cameraPosition.length);
		
		float[] cameraViewDirection = new float[3];
		cameraViewDirection[0] = player.GetViewDirection().x;
		cameraViewDirection[1] = player.GetViewDirection().y;
		cameraViewDirection[2] = player.GetViewDirection().z;
		shaderProgram.setUniform3fv("u_cameraViewDirection", cameraViewDirection, 0, cameraViewDirection.length);
		
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
