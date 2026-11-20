.class Lcom/racas/black/CinemaActivity$2;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/view/View$OnClickListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->showList()V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;

.field final synthetic val$index:I


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;I)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010,
            0x1010
        }
        names = {
            null,
            null
        }
    .end annotation

    .annotation system Ldalvik/annotation/Signature;
        value = {
            "()V"
        }
    .end annotation

    .line 80
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$2;->this$0:Lcom/racas/black/CinemaActivity;

    iput p2, p0, Lcom/racas/black/CinemaActivity$2;->val$index:I

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onClick(Landroid/view/View;)V
    .locals 2

    .line 80
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$2;->this$0:Lcom/racas/black/CinemaActivity;

    iget v0, p0, Lcom/racas/black/CinemaActivity$2;->val$index:I

    const/4 v1, 0x0

    invoke-static {p1, v0, v1}, Lcom/racas/black/CinemaActivity;->access$000(Lcom/racas/black/CinemaActivity;II)V

    return-void
.end method
