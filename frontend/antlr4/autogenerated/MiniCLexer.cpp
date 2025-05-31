
// Generated from /home/code/NPU_CPExperiments/frontend/antlr4/MiniC.g4 by ANTLR 4.12.0


#include "MiniCLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct MiniCLexerStaticData final {
  MiniCLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  MiniCLexerStaticData(const MiniCLexerStaticData&) = delete;
  MiniCLexerStaticData(MiniCLexerStaticData&&) = delete;
  MiniCLexerStaticData& operator=(const MiniCLexerStaticData&) = delete;
  MiniCLexerStaticData& operator=(MiniCLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag miniclexerLexerOnceFlag;
MiniCLexerStaticData *miniclexerLexerStaticData = nullptr;

void miniclexerLexerInitialize() {
  assert(miniclexerLexerStaticData == nullptr);
  auto staticData = std::make_unique<MiniCLexerStaticData>(
    std::vector<std::string>{
      "T_L_PAREN", "T_R_PAREN", "T_SEMICOLON", "T_L_BRACE", "T_R_BRACE", 
      "T_ASSIGN", "T_COMMA", "T_ADD", "T_SUB", "T_MUL", "T_DIV", "T_MOD", 
      "T_LT", "T_GT", "T_LE", "T_GE", "T_EQ", "T_NEQ", "T_AND", "T_OR", 
      "T_NOT", "T_RETURN", "T_INT", "T_VOID", "T_BOOL", "T_IF", "T_ELSE", 
      "T_WHILE", "T_BREAK", "T_CONTINUE", "T_ID", "T_DIGIT", "WS", "LINE_COMMENT"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'('", "')'", "';'", "'{'", "'}'", "'='", "','", "'+'", "'-'", 
      "'*'", "'/'", "'%'", "'<'", "'>'", "'<='", "'>='", "'=='", "'!='", 
      "'&&'", "'||'", "'!'", "'return'", "'int'", "'void'", "'bool'", "'if'", 
      "'else'", "'while'", "'break'", "'continue'"
    },
    std::vector<std::string>{
      "", "T_L_PAREN", "T_R_PAREN", "T_SEMICOLON", "T_L_BRACE", "T_R_BRACE", 
      "T_ASSIGN", "T_COMMA", "T_ADD", "T_SUB", "T_MUL", "T_DIV", "T_MOD", 
      "T_LT", "T_GT", "T_LE", "T_GE", "T_EQ", "T_NEQ", "T_AND", "T_OR", 
      "T_NOT", "T_RETURN", "T_INT", "T_VOID", "T_BOOL", "T_IF", "T_ELSE", 
      "T_WHILE", "T_BREAK", "T_CONTINUE", "T_ID", "T_DIGIT", "WS", "LINE_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,34,219,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,1,0,1,0,1,1,1,
  	1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,1,6,1,6,1,7,1,7,1,8,1,8,1,9,1,9,1,10,
  	1,10,1,11,1,11,1,12,1,12,1,13,1,13,1,14,1,14,1,14,1,15,1,15,1,15,1,16,
  	1,16,1,16,1,17,1,17,1,17,1,18,1,18,1,18,1,19,1,19,1,19,1,20,1,20,1,21,
  	1,21,1,21,1,21,1,21,1,21,1,21,1,22,1,22,1,22,1,22,1,23,1,23,1,23,1,23,
  	1,23,1,24,1,24,1,24,1,24,1,24,1,25,1,25,1,25,1,26,1,26,1,26,1,26,1,26,
  	1,27,1,27,1,27,1,27,1,27,1,27,1,28,1,28,1,28,1,28,1,28,1,28,1,29,1,29,
  	1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,30,1,30,5,30,170,8,30,10,30,12,30,
  	173,9,30,1,31,1,31,1,31,4,31,178,8,31,11,31,12,31,179,1,31,1,31,1,31,
  	1,31,3,31,186,8,31,1,31,4,31,189,8,31,11,31,12,31,190,1,31,1,31,5,31,
  	195,8,31,10,31,12,31,198,9,31,3,31,200,8,31,1,32,4,32,203,8,32,11,32,
  	12,32,204,1,32,1,32,1,33,1,33,1,33,1,33,5,33,213,8,33,10,33,12,33,216,
  	9,33,1,33,1,33,0,0,34,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,
  	11,23,12,25,13,27,14,29,15,31,16,33,17,35,18,37,19,39,20,41,21,43,22,
  	45,23,47,24,49,25,51,26,53,27,55,28,57,29,59,30,61,31,63,32,65,33,67,
  	34,1,0,8,3,0,65,90,95,95,97,122,4,0,48,57,65,90,95,95,97,122,1,0,48,55,
  	3,0,48,57,65,70,97,102,1,0,49,57,1,0,48,57,3,0,9,10,13,13,32,32,2,0,10,
  	10,13,13,228,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,
  	0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,
  	0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,
  	1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,0,
  	0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,1,0,0,0,0,49,1,0,0,0,0,51,1,0,0,0,
  	0,53,1,0,0,0,0,55,1,0,0,0,0,57,1,0,0,0,0,59,1,0,0,0,0,61,1,0,0,0,0,63,
  	1,0,0,0,0,65,1,0,0,0,0,67,1,0,0,0,1,69,1,0,0,0,3,71,1,0,0,0,5,73,1,0,
  	0,0,7,75,1,0,0,0,9,77,1,0,0,0,11,79,1,0,0,0,13,81,1,0,0,0,15,83,1,0,0,
  	0,17,85,1,0,0,0,19,87,1,0,0,0,21,89,1,0,0,0,23,91,1,0,0,0,25,93,1,0,0,
  	0,27,95,1,0,0,0,29,97,1,0,0,0,31,100,1,0,0,0,33,103,1,0,0,0,35,106,1,
  	0,0,0,37,109,1,0,0,0,39,112,1,0,0,0,41,115,1,0,0,0,43,117,1,0,0,0,45,
  	124,1,0,0,0,47,128,1,0,0,0,49,133,1,0,0,0,51,138,1,0,0,0,53,141,1,0,0,
  	0,55,146,1,0,0,0,57,152,1,0,0,0,59,158,1,0,0,0,61,167,1,0,0,0,63,199,
  	1,0,0,0,65,202,1,0,0,0,67,208,1,0,0,0,69,70,5,40,0,0,70,2,1,0,0,0,71,
  	72,5,41,0,0,72,4,1,0,0,0,73,74,5,59,0,0,74,6,1,0,0,0,75,76,5,123,0,0,
  	76,8,1,0,0,0,77,78,5,125,0,0,78,10,1,0,0,0,79,80,5,61,0,0,80,12,1,0,0,
  	0,81,82,5,44,0,0,82,14,1,0,0,0,83,84,5,43,0,0,84,16,1,0,0,0,85,86,5,45,
  	0,0,86,18,1,0,0,0,87,88,5,42,0,0,88,20,1,0,0,0,89,90,5,47,0,0,90,22,1,
  	0,0,0,91,92,5,37,0,0,92,24,1,0,0,0,93,94,5,60,0,0,94,26,1,0,0,0,95,96,
  	5,62,0,0,96,28,1,0,0,0,97,98,5,60,0,0,98,99,5,61,0,0,99,30,1,0,0,0,100,
  	101,5,62,0,0,101,102,5,61,0,0,102,32,1,0,0,0,103,104,5,61,0,0,104,105,
  	5,61,0,0,105,34,1,0,0,0,106,107,5,33,0,0,107,108,5,61,0,0,108,36,1,0,
  	0,0,109,110,5,38,0,0,110,111,5,38,0,0,111,38,1,0,0,0,112,113,5,124,0,
  	0,113,114,5,124,0,0,114,40,1,0,0,0,115,116,5,33,0,0,116,42,1,0,0,0,117,
  	118,5,114,0,0,118,119,5,101,0,0,119,120,5,116,0,0,120,121,5,117,0,0,121,
  	122,5,114,0,0,122,123,5,110,0,0,123,44,1,0,0,0,124,125,5,105,0,0,125,
  	126,5,110,0,0,126,127,5,116,0,0,127,46,1,0,0,0,128,129,5,118,0,0,129,
  	130,5,111,0,0,130,131,5,105,0,0,131,132,5,100,0,0,132,48,1,0,0,0,133,
  	134,5,98,0,0,134,135,5,111,0,0,135,136,5,111,0,0,136,137,5,108,0,0,137,
  	50,1,0,0,0,138,139,5,105,0,0,139,140,5,102,0,0,140,52,1,0,0,0,141,142,
  	5,101,0,0,142,143,5,108,0,0,143,144,5,115,0,0,144,145,5,101,0,0,145,54,
  	1,0,0,0,146,147,5,119,0,0,147,148,5,104,0,0,148,149,5,105,0,0,149,150,
  	5,108,0,0,150,151,5,101,0,0,151,56,1,0,0,0,152,153,5,98,0,0,153,154,5,
  	114,0,0,154,155,5,101,0,0,155,156,5,97,0,0,156,157,5,107,0,0,157,58,1,
  	0,0,0,158,159,5,99,0,0,159,160,5,111,0,0,160,161,5,110,0,0,161,162,5,
  	116,0,0,162,163,5,105,0,0,163,164,5,110,0,0,164,165,5,117,0,0,165,166,
  	5,101,0,0,166,60,1,0,0,0,167,171,7,0,0,0,168,170,7,1,0,0,169,168,1,0,
  	0,0,170,173,1,0,0,0,171,169,1,0,0,0,171,172,1,0,0,0,172,62,1,0,0,0,173,
  	171,1,0,0,0,174,200,5,48,0,0,175,177,5,48,0,0,176,178,7,2,0,0,177,176,
  	1,0,0,0,178,179,1,0,0,0,179,177,1,0,0,0,179,180,1,0,0,0,180,200,1,0,0,
  	0,181,182,5,48,0,0,182,186,5,120,0,0,183,184,5,48,0,0,184,186,5,88,0,
  	0,185,181,1,0,0,0,185,183,1,0,0,0,186,188,1,0,0,0,187,189,7,3,0,0,188,
  	187,1,0,0,0,189,190,1,0,0,0,190,188,1,0,0,0,190,191,1,0,0,0,191,200,1,
  	0,0,0,192,196,7,4,0,0,193,195,7,5,0,0,194,193,1,0,0,0,195,198,1,0,0,0,
  	196,194,1,0,0,0,196,197,1,0,0,0,197,200,1,0,0,0,198,196,1,0,0,0,199,174,
  	1,0,0,0,199,175,1,0,0,0,199,185,1,0,0,0,199,192,1,0,0,0,200,64,1,0,0,
  	0,201,203,7,6,0,0,202,201,1,0,0,0,203,204,1,0,0,0,204,202,1,0,0,0,204,
  	205,1,0,0,0,205,206,1,0,0,0,206,207,6,32,0,0,207,66,1,0,0,0,208,209,5,
  	47,0,0,209,210,5,47,0,0,210,214,1,0,0,0,211,213,8,7,0,0,212,211,1,0,0,
  	0,213,216,1,0,0,0,214,212,1,0,0,0,214,215,1,0,0,0,215,217,1,0,0,0,216,
  	214,1,0,0,0,217,218,6,33,0,0,218,68,1,0,0,0,9,0,171,179,185,190,196,199,
  	204,214,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  miniclexerLexerStaticData = staticData.release();
}

}

MiniCLexer::MiniCLexer(CharStream *input) : Lexer(input) {
  MiniCLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *miniclexerLexerStaticData->atn, miniclexerLexerStaticData->decisionToDFA, miniclexerLexerStaticData->sharedContextCache);
}

MiniCLexer::~MiniCLexer() {
  delete _interpreter;
}

std::string MiniCLexer::getGrammarFileName() const {
  return "MiniC.g4";
}

const std::vector<std::string>& MiniCLexer::getRuleNames() const {
  return miniclexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& MiniCLexer::getChannelNames() const {
  return miniclexerLexerStaticData->channelNames;
}

const std::vector<std::string>& MiniCLexer::getModeNames() const {
  return miniclexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& MiniCLexer::getVocabulary() const {
  return miniclexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView MiniCLexer::getSerializedATN() const {
  return miniclexerLexerStaticData->serializedATN;
}

const atn::ATN& MiniCLexer::getATN() const {
  return *miniclexerLexerStaticData->atn;
}




void MiniCLexer::initialize() {
  ::antlr4::internal::call_once(miniclexerLexerOnceFlag, miniclexerLexerInitialize);
}
