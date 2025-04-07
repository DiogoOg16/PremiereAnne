package com.example.mea4_ogochi_diogo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.navigation.compose.*
import com.example.mea4_ogochi_diogo.ui.theme.MEA4OgochiDiogoTheme
import kotlinx.coroutines.delay

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            MEA4OgochiDiogoTheme {
                val navController = rememberNavController()
                NavHost(navController = navController, startDestination = "game") {
                    composable("game") {
                        ColorGameScreen(onAboutClick = {
                            navController.navigate("about")
                        })
                    }
                    composable("about") {
                        AboutScreen(onBackClick = {
                            navController.popBackStack()
                        })
                    }
                }
            }
        }
    }
}

@Composable
fun ColorGameScreen(modifier: Modifier = Modifier, onAboutClick: () -> Unit) {
    val colors = listOf("ROUGE", "VERT", "BLEU", "JAUNE")
    val colorMap = mapOf(
        "ROUGE" to Color(0xFFF94144),
        "VERT" to Color(0xFF43AA8B),
        "BLEU" to Color(0xFF577590),
        "JAUNE" to Color(0xFFF9C74F)
    )

    val squareShape = RoundedCornerShape(16.dp)
    val playButtonShape = RoundedCornerShape(12.dp)
    val titleColor = Color.White
    val labelColor = Color(0xFF90E0EF)
    val errorColor = Color(0xFFFF6B6B)

    var level by remember { mutableStateOf(0) }
    var highScore by remember { mutableStateOf(0) }
    val sequence = remember { mutableStateListOf<String>() }
    val userInputs = remember { mutableStateListOf<String>() }
    var currentDisplayColor by remember { mutableStateOf("") }
    var displayTextColor by remember { mutableStateOf(titleColor) }
    var isShowingSequence by remember { mutableStateOf(false) }
    var gameOver by remember { mutableStateOf(false) }

    fun getRandomMismatchedColor(name: String): Color {
        val otherNames = colors.filter { it != name }
        val randomName = otherNames.random()
        return colorMap[randomName] ?: titleColor
    }

    fun startGame() {
        level = 1
        highScore = maxOf(highScore, level)
        sequence.clear()
        sequence.add(colors.random())
        userInputs.clear()
        gameOver = false
        isShowingSequence = true
    }

    fun handleUserTap(color: String) {
        if (isShowingSequence || gameOver) return
        userInputs.add(color)
        if (userInputs.size == sequence.size) {
            if (userInputs.toList() == sequence.toList()) {
                level++
                if (level > highScore) {
                    highScore = level
                }
                sequence.add(colors.random())
                userInputs.clear()
                isShowingSequence = true
            } else {
                gameOver = true
                currentDisplayColor = "Game Over"
                displayTextColor = errorColor
            }
        }
    }

    LaunchedEffect(isShowingSequence, level) {
        if (isShowingSequence) {
            for (color in sequence) {
                currentDisplayColor = color
                displayTextColor = getRandomMismatchedColor(color)
                delay(600)
                currentDisplayColor = ""
                delay(200)
            }
            currentDisplayColor = ""
            displayTextColor = titleColor
            isShowingSequence = false
        }
    }

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF1B2B44))
            .padding(24.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text("Jeu de Mémoire", fontSize = 28.sp, fontWeight = FontWeight.Bold, color = titleColor)

        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            Text("Niveau: $level", color = labelColor, fontSize = 20.sp, fontWeight = FontWeight.SemiBold)
            Text("Meilleur niveau: $highScore", color = labelColor, fontSize = 18.sp, fontWeight = FontWeight.Medium)
        }

        Box(
            modifier = Modifier
                .height(40.dp)
                .fillMaxWidth(),
            contentAlignment = Alignment.Center
        ) {
            if (currentDisplayColor.isNotEmpty()) {
                Text(currentDisplayColor, fontSize = 24.sp, fontWeight = FontWeight.Bold, color = displayTextColor)
            }
        }

        Column(
            verticalArrangement = Arrangement.spacedBy(8.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp), modifier = Modifier.fillMaxWidth()) {
                listOf("VERT", "ROUGE").forEach { color ->
                    Box(
                        modifier = Modifier
                            .weight(1f)
                            .aspectRatio(1f)
                            .background(colorMap[color]!!, squareShape)
                            .clickable(enabled = !isShowingSequence && !gameOver) {
                                handleUserTap(color)
                            }
                    )
                }
            }
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp), modifier = Modifier.fillMaxWidth()) {
                listOf("JAUNE", "BLEU").forEach { color ->
                    Box(
                        modifier = Modifier
                            .weight(1f)
                            .aspectRatio(1f)
                            .background(colorMap[color]!!, squareShape)
                            .clickable(enabled = !isShowingSequence && !gameOver) {
                                handleUserTap(color)
                            }
                    )
                }
            }
        }

        TextButton(onClick = onAboutClick) {
            Text("À propos", color = labelColor)
        }

        Button(
            onClick = { startGame() },
            modifier = Modifier
                .width(180.dp)
                .height(48.dp),
            shape = playButtonShape,
            colors = ButtonDefaults.buttonColors(containerColor = Color(0xFFE63946))
        ) {
            Text(if (gameOver) "PLAY AGAIN!" else "PLAY", color = Color.White)
        }
    }
}

@Composable
fun AboutScreen(onBackClick: () -> Unit) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF1B2B44))
            .padding(32.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.SpaceBetween
    ) {
        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            Text("À propos", fontSize = 28.sp, fontWeight = FontWeight.Bold, color = Color.White)
            Spacer(modifier = Modifier.height(16.dp))
            Text(
                "Jeu de Mémoire\nRéalisé par: Ton Nom\nProjet MEA4",
                fontSize = 18.sp,
                color = Color(0xFF90E0EF),
                textAlign = TextAlign.Center
            )
        }

        Button(
            onClick = onBackClick,
            modifier = Modifier
                .width(160.dp)
                .height(48.dp),
            shape = RoundedCornerShape(12.dp),
            colors = ButtonDefaults.buttonColors(containerColor = Color(0xFFE63946))
        ) {
            Text("Retour", color = Color.White)
        }
    }
}
